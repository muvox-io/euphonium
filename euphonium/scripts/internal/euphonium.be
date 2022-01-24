class EuphoniumInstance
    var event_handlers
    var plugins
    var playback_state
    var plugins_initialized
    var network_state
    var current_source

    # Setup initial values
    def init()
        self.plugins_initialized = false
        self.event_handlers = {
            'handleRouteEvent': def (request)
                http.handle_event(request)
            end,
            'songChangedEvent': def (song)
                self.update_song(song)
            end,
            'audioTakeoverEvent': def (req)
                self.current_source = req['source']
                self.send_notification("success", self.current_source, "Took over playback")
            end,
            'playbackError': def (req)
                self.current_source = req['source']
                self.send_notification("error", self.current_source, req["errorMessage"])
            end,
            'statusChangedEvent': def (req)
                if req['isPaused']
                    self.playback_state['status'] = 'paused'
                else
                    self.playback_state['status'] = 'playing'
                end

                self.update_playback()
            end,
            'youtubeEvent': def (ev)
                self.get_plugin('youtube').on_event('youtube', ev)
            end,
            'volumeChangedEvent': def (req)
                self.apply_volume(int(req['volume']))
            end,
            'handleConfigLoaded': def (config)
                self.load_configuration_for(config)
            end
        }
        self.plugins = []
        self.playback_state = {
            'song': {
                'songName': 'Queue empty',
                'artistName': '--',
                'sourceName': '--',
                'icon': '',
                'albumName': '--'
            },
            'eq': {
                'low': 0,
                'mid': 0,
                'high': 0
            },
            'volume': 50,
            'status': 'paused'
        }
        self.network_state = 'offline'
    end

    # Registers a new event handler
    def register_handler(type, handler)
        self.event_handlers[type] = handler
    end

    # Pass event to given handler
    def handle_event(eventType, eventData)
        if (self.event_handlers[eventType] != nil)
            self.event_handlers[eventType](eventData)
        end
    end

    def update_song(playback_info)
        self.playback_state['song'] = playback_info

        if playback_info['sourceName'] != ''
            self.playback_state['song']['sourceThemeColor'] = self.get_plugin(playback_info['sourceName']).theme_color
        end
        self.update_playback()
    end

    def set_status(playback_status)
        self.playback_state['status'] = playback_status
        self.update_playback()
    end

    def update_playback()
        http.emit_event("playback", self.playback_state)
    end

    def send_notification(type, from, text, submessage)
        var second_message = ""
        if submessage != nil 
            second_message = submessage
        end
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
        var plugin = self.get_audio_output()
        if (plugin != nil)
            plugin.on_event(EVENT_CONFIG_UPDATED, {})
            plugin.init_audio()
        end

        core.start_plugin('cspot', self.get_plugin('cspot').get_raw_config())
        core.start_plugin('webradio', self.get_plugin('webradio').get_raw_config())
        core.start_plugin('bluetooth', {})

        self.init_http()
    end

    # Starts the HTTP thread
    def init_http()
        core.start_plugin('http', {})
    end

    # Adds plugin to plugin registry
    def register_plugin(plugin)
        self.plugins.push(plugin)
    end

    def load_configuration()
        for plugin : self.plugins
            persistor.load(plugin.name + ".config.json")
        end
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
        var str_index = string.find(conf['key'], ".config.json")
        if (str_index > 0)
            var plugin_name = string.split(conf['key'], str_index)[0]
            plugin = self.get_plugin(plugin_name)
            plugin.load_config(conf['value'])
            plugin.configuration_loaded = true
            self.load_plugins_when_ready()
        end
    end

    def load_plugins_when_ready()
        if (!self.plugins_initialized)
            for plugin : self.plugins
                if (!plugin.configuration_loaded)
                    return
                end
            end

            self.plugins_initialized = true

            for plugin : self.plugins
                if plugin.type == 'init_handler'
                    plugin.on_event(EVENT_SYSTEM_INIT, {})
                    return
                end
            end

            if core.platform() == 'desktop'
                self.init_required_plugins()
            end
        end
    end

    def apply_volume(volume)
        if core.platform() == 'desktop'
            playback.set_soft_volume(volume)
        else
            var dac_plugin = self.get_plugin('dac')
            if !dac_plugin.has_hardware_volume()
                playback.set_soft_volume(volume)
            end
        end

        self.playback_state['volume'] = volume
        self.update_playback()
        print("Broadcasting volume data")
        self.broadcast_event(EVENT_VOLUME_UPDATED, volume)
    end
end

euphonium = EuphoniumInstance()

# Native closures
def handle_event(event_type, event_data)
    euphonium.handle_event(event_type, event_data)
end

def load_plugins()
    print("Load plugins called")
    euphonium.load_configuration()
end

core.start_plugin('persistor', {})
