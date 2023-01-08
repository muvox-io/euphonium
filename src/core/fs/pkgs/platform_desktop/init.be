class PortAudioPlugin : Plugin
    def init()
        self.apply_default_values()

        self.name = "portaudio"
        self.display_name = "PortAudio output"
        self.type = "system"
        self.is_audio_output = true

        self.fetch_config()
    end

    def make_form(ctx, state)
        ctx.create_group('driver', { 'label': 'Output settings' })  
    end

    def on_event(event, data)
    end
end

euphonium.register_plugin(PortAudioPlugin())
