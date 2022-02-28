class CSpotPlugin : Plugin
    def init()
        self.apply_default_values()
        self.name = "cspot"
        self.theme_color = "#1DB954"
        self.display_name = "Spotify (cspot)"
        self.type = "plugin"
    end

    def make_form(ctx, state)
        ctx.create_group('spotify', { 'label': 'General' })

        ctx.text_field('receiverName', {
            'label': "Speaker's name",
            'default': "Euphonium",
            'group': 'spotify'
        })
        ctx.select_field('audioBitrate', {
            'label': "Audio bitrate",
            'default': "160",
            'values': ['320', '160', '96'],
            'group': 'spotify'
        })
    end

    def on_event(event, data)
        if event == EVENT_SET_PAUSE
            cspot_set_pause(data)
        end

        if event == EVENT_VOLUME_UPDATED
            cspot_set_volume_remote(data)
        end
    end
end

euphonium.register_plugin(CSpotPlugin())
