
class BluetoothPlugin : Plugin
    def init()
        self.configSchema = {
            'name': {
                'tooltip': 'Device\'s name',
                'type': 'string',
                'defaultValue': "Euphonium"
            },
        }

        self.applyDefaultValues()
        self.name = "bluetooth"
        self.displayName = "Bluetooth"
        self.type = "plugin"
    end
end

app.registerPlugin(BluetoothPlugin())
