class SnapcastPlugin : Plugin
    def init()
        self.apply_default_values()
        self.name = "snapcast"
        self.theme_color = "#d2c464"
        self.display_name = "Snapcast Multiroom"
        self.type = "plugin"
        self.has_web_app = false
    end

    def make_form(ctx, state)
        ctx.create_group('snapcast', { 'label': 'General' })

        ctx.text_field('serverUrl', {
            'label': "Server instance url",
            'default': "http://localhost:9000",
            'group': 'snapcast'
        })
    end
end

euphonium.register_plugin(SnapcastPlugin())
