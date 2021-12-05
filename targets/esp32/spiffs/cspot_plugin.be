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
end

app.registerPlugin(CSpotPlugin())
