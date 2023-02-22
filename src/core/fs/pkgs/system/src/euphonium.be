class EuphoniumInstance
    var plugins
    var plugins_initialized

    # Setup initial values
    def init()
        #self.last_volume = 0
        self.plugins_initialized = false
        self.plugins = []
        
        events.register_native("plugins_ready", def (data)
            self.broadcast_event(EVENT_PLUGIN_INIT, {})
        end)

        events.register_native("volume", def (data)
          if data["source"] == "remote"
            self.apply_volume(int(data["value"]))
            playback_state.update_remote()
          end
        end)
    end

    # Registers a new event handler
    def register_handler(type, handler)
        # self.event_handlers[type] = handler
    end

    def update_song(playback_info)
        # self.playback_state['song'] = playback_info

        # if playback_info['sourceName'] != ''
        #     self.playback_state['song']['sourceThemeColor'] = self.get_plugin(playback_info['sourceName']).theme_color
        # end
        # self.update_playback()
    end

    def set_status(playback_status)
        # self.playback_state['status'] = playback_status
        # self.update_playback()
    end

    def update_playback()
        # http.emit_event("playback", self.playback_state)
    end

    def send_notification(type, from, text, submessage)
        http.emit_event("notification", { 'type': type, 'message': text, 'source': from, 'submessage': submessage })
    end

    def get_audio_output()
        for plugin : self.plugins
            if (plugin.is_audio_output)
                return plugin
            end
        end

        return nil
    end

    # Initializes all plugins
    def init_required_plugins()
        self.network_state = 'online'
    end

    def init_plugin(plugin_name)
        plugin = self.get_plugin(plugin_name)
        if (plugin != nil)
            var ctx = FormContext()
            plugin.make_form(ctx, plugin.state)
            ctx.apply_state(plugin.state)
    
            var raw_state = {}
            for field : ctx.fields
                if field['type'] != 'group'
                    raw_state[field['key']] = field['value']
                end
            end

            # raw_state['volume'] = self.playback_state['volume'];
            core.start_plugin(plugin_name, raw_state)
        end
    end

    # Adds plugin to plugin registry
    def register_plugin(plugin)
        self.plugins.push(plugin)
    end

    def persist_playback_state()
        # persistor.persist("configuration/playback.config.json", json.dump(self.playback_state))
    end

    def load_configuration()
        # for plugin : self.plugins
        #     persistor.load("configuration/" + plugin.name + ".config.json")
        # end
    end

    # Returns plugin with given name
    def get_plugin(name)
        for plugin : self.plugins
            if (plugin.name == name) 
                return plugin
            end
        end

        return nil
    end

    # broadcasts given event to all plugins
    def broadcast_event(event_type, event_data)
        for plugin : self.plugins
            plugin.on_event(event_type, event_data)
        end
    end

    # sends an event to a particular plugin
    def send_plugin_event(plugin, event_type, event_data)
        plugin = self.get_plugin(plugin)
        plugin.on_event(event_type, event_data)
    end

    # loads configuration for given plugin
    def load_configuration_for(conf)
        # var str_index = string.find(conf['key'], ".config.json")
        # if (str_index > 0)
        #     var plugin_name = string.split(conf['key'], str_index)[0]
        #     plugin_name = string.split(plugin_name, string.find(plugin_name, "/") + 1)[1]

        #     if plugin_name == "playback"
        #         if conf['value'] != ''
        #             self.playback_state = json.load(conf['value'])
        #         end
        #     else 
        #         plugin = self.get_plugin(plugin_name)
        #         plugin.load_config(conf['value'])
        #         plugin.configuration_loaded = true
        #         self.load_plugins_when_ready()
        #     end
        # end
    end

    def load_plugins_when_ready()
        # if (!self.plugins_initialized)
        #     for plugin : self.plugins
        #         if (!plugin.configuration_loaded)
        #             return
        #         end
        #     end

        #     self.plugins_initialized = true

        #     for plugin : self.plugins
        #         if plugin.type == 'init_handler'
        #             plugin.on_event(EVENT_SYSTEM_INIT, {})
        #         end
        #     end

        #     var pluginAudio = self.get_audio_output()
        #     if (pluginAudio != nil)
        #         pluginAudio.on_event(EVENT_CONFIG_UPDATED, {})
        #         pluginAudio.init_audio()
        #     end

        #     self.init_plugin('bluetooth')
        #     if core.platform() == 'desktop'
        #         self.init_required_plugins()
        #     end

        #     if self.playback_state != nil
        #         self.apply_volume(self.playback_state['volume'])
        #     end
        # end
    end

    def apply_volume(volume)
      if volume >= 0 && volume <= 100
        var hardware_plugin = self.get_plugin('hardware')

        core.set_native_volume(volume)
        playback_state.update_volume(volume)
        self.broadcast_event(EVENT_VOLUME_UPDATED, volume)
      end
    end
end

euphonium = EuphoniumInstance()
