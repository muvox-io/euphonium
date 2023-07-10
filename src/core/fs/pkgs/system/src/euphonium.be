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

        events.register_native("notification", def (data)
            self.send_notification(data["type"], data["source"], data["message"], data["submessage"])
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

    # Adds plugin to plugin registry
    def register_plugin(plugin)
        self.plugins.push(plugin)
    end

    def persist_playback_state()
        # persistor.persist("configuration/playback.config.json", json.dump(self.playback_state))
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
