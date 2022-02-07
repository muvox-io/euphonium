import string

EVENT_CONFIG_UPDATED = 'conf_updated'
EVENT_VOLUME_UPDATED = 'volume_updated'
EVENT_SYSTEM_INIT = 'system_init'
EVENT_SET_PAUSE = 'set_pause'

class Plugin
    var name
    var type
    var display_name
    var config_schema
    var is_audio_output
    var has_web_app
    var configuration_loaded
    var theme_color

    # Call in init, sets default values
    def apply_default_values()
        self.has_web_app = false
        self.configuration_loaded = false
        self.is_audio_output = false
        self.theme_color = "#fff"

        for key : self.config_schema.keys()
            self.config_schema[key]['value'] = self.config_schema[key]['defaultValue']
        end
    end

    # Called to initialize audio when `is_audio_output` == true
    def init_audio()
    end

    def on_event(event, data)
    end

    # Returns value of a given config key
    def config_value(key)
        return self.config_schema[key]['value']
    end

    # Returns value-only state of config schema
    def get_raw_config()
        var bare_values = {}
        for key : self.config_schema.keys()
            bare_values[key] = self.config_schema[key]['value']
        end

        return bare_values
    end

    # saves raw confugration into memory
    def persist_config()
        var bare_values = self.get_raw_config()
        persistor.persist("configuration/" + self.name + ".config.json", json.dump(bare_values))
    end

    # Loads raw configuration from schema
    def load_config(schema)
        var schema_obj = json.load(schema)
        if (schema_obj == nil) 
            schema_obj = {}
        end

        for key : schema_obj.keys()
            self.config_schema[key]['value'] = schema_obj[key]
        end
    end
end