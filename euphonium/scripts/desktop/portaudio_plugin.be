class PortAudioPlugin : Plugin
    def init()
        self.configSchema = {
        }
        self.applyDefaultValues()
        self.name = "wifi"
        self.displayName = "WiFi"
        self.type = "system_hidden"
    end
end

app.registerPlugin(PortAudioPlugin())
