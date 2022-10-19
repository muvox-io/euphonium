class I2SDriver : DACDriver
    def init()
        self.name = "I2S"
        self.hardware_volume_control = false
    end

    def init_i2s()
        # I2S STAND MODE
        var config = I2SConfig()
        config.sample_rate = 44100
        config.bits_per_sample = 16
        config.mclk = 0
        config.comm_format = I2S_CHANNEL_FMT_RIGHT_LEFT
        config.channel_format = I2S_COMM_FORMAT_I2S

        i2s.install(config)
        i2s.set_pins(self.get_i2s_pins())
    end

    def unload_i2s()
        i2s.uninstall()
    end

    def set_volume(volume)
    end

    def make_config_form(ctx, state) 
        super(self).make_config_form(ctx, state)
    end
end

hardware.register_driver(I2SDriver())
