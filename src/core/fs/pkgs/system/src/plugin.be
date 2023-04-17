import string

EVENT_CONFIG_UPDATED = 'conf_updated'
EVENT_VOLUME_UPDATED = 'volume_updated'
EVENT_SYSTEM_INIT = 'system_init'
EVENT_SET_PAUSE = 'set_pause'
EVENT_PLUGIN_INIT = 'plugin_init'

class Plugin
    var name
    var type
    var display_name
    var config_schema
    var is_audio_output
    var has_web_app
    var configuration_loaded
    var theme_color
    var state

    # Call in init, sets default values
    def apply_default_values()
        self.has_web_app = false
        self.configuration_loaded = false
        self.is_audio_output = false
        self.theme_color = "#fff"
        self.state = {}
        var ctx = FormContext()
        self.make_form(ctx, self.state)
        var all_fields = ctx.children
        # walk fields recursively
        ctx.walk(def (data)
            if data.find("default") != nil && data.find("stateKey") != nil
                self.state[data["stateKey"]] = data["default"]
            end
        end)
    end

    # Called to initialize audio when `is_audio_output` == true
    def init_audio()
    end

    def on_event(event, data)
    end

    # Returns value of a given config key
    def config_value(key)
        return self.state[key]
    end

    # Returns value-only state of config schema
    def get_raw_config()
        return self.state
    end

    # saves raw confugration to disk
    def persist_config()
        core.save_config(self.name, json.dump(self.state))
    end

    # utility function to add apply button to a form
    def add_apply_button(ctx, state)
        if ctx.button_field("_apply_button", {
            "label": "",
            "buttonText": "Apply changes"
        }).has_been("click")
            self.state = state
            self.persist_config()
            # todo: rework api so that no circular dependency
            http.emit_event("notification", 
                { 
                    'type': 'info',
                    'message': "Configuration updated",
                    'source': self.name,
                }
            )
            print("asdf")
            self.on_event(EVENT_CONFIG_UPDATED, self.state)
        end
    end

    # load plugin configuration from disk
    def fetch_config()
        var config = core.load_config(self.name)
        var loaded_state = json.load(config)
        for key : loaded_state.keys()
          self.state[key] = loaded_state[key]
        end
    end

    def make_form(ctx, state)
    end

    # Loads raw configuration from schema
    def load_config(schema)
        var schema_obj = json.load(schema)
        if (schema_obj == nil) 
            schema_obj = {}
        end

        for key : schema_obj.keys()
            self.state = schema_obj
        end
    end
end
