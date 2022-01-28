
class BluetoothPlugin : Plugin
    def init()
        self.config_schema = {
            'name': {
                'tooltip': 'Device\'s name',
                'type': 'string',
                'defaultValue': "Euphonium"
            },
        }

        self.apply_default_values()
        self.theme_color = "#287AA9"
        self.name = "bluetooth"
        self.display_name = "Bluetooth"
        self.type = "plugin"
    end
end

euphonium.register_plugin(BluetoothPlugin())
