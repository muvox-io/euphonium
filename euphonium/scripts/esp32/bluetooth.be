
class BluetoothPlugin : Plugin

    
    def init()
        self.apply_default_values()
        self.theme_color = "#287AA9"
        self.name = "bluetooth"
        self.display_name = "Bluetooth"
        self.type = "plugin"
    end

    def make_form(ctx, state)
        ctx.create_group('bluetooth', { 'label': 'General' })

        ctx.text_field('name', {
            'label': "Device's name",
            'default': util.generate_device_name(),
            'group': 'bluetooth'
        })

        ctx.checkbox_field('usePin', {
            'label': "Require PIN to connect",
            'default': "false",
            'group': 'bluetooth'
        })

        if state.find('usePin') != nil && state['usePin'] == 'true'
            ctx.text_field('pin', {
                'label': "Bluetooth PIN (restart to apply)",
                'default': "2137",
                'group': 'bluetooth'
            })
        end
    end
end

euphonium.register_plugin(BluetoothPlugin())
