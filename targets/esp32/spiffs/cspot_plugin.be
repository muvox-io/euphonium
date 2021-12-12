class CSpotPlugin : Plugin
    def init()
        self.configSchema = {
            'receiverName': {
                'tooltip': "Speaker's name",
                'type': 'string',
                'defaultValue': 'Euphonium (cspot)'
            },
            'audioBitrate': {
                'tooltip': 'Audio bitrate',
                'type': 'stringList',
                'listValues': ["96", "160", "320"],
                'defaultValue': '160'
            },
        }

        self.applyDefaultValues()
        self.name = "cspot"
        self.displayName = "Spotify (cspot)"
        self.type = "plugin"
    end

    def onEvent(event, data)
        if event == EVENT_SET_PAUSE
            cspot_set_pause(data)
        end

        if event == EVENT_CONFIG_UPDATED
            cspot_config_updated()
        end

        if event == EVENT_VOLUME_UPDATED
            cspot_set_volume_remote(data)
        end
    end
end

app.registerPlugin(CSpotPlugin())
