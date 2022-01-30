class ES8388Driver : DACDriver
    def init()
        self.name = "ES8388"
        self.has_hardware_volume = true
    end

    def init_i2s()
        var ADDRESS = 0x10

        var config = I2SConfig()
        config.sample_rate = 44100
        config.bits_per_sample = 16
        config.mclk = 0
        config.comm_format = I2S_CHANNEL_FMT_RIGHT_LEFT
        config.channel_format = I2S_COMM_FORMAT_I2S

        i2s.install(config)
        i2s.set_pins(self.get_i2s_pins())

        i2c.install(int(self.get_gpio('sda')), int(self.get_gpio('scl')))
        sleep_ms(50)
        
        # init
        i2c.write(ADDRESS, 0x08, 0x00) # set slave mode
        i2c.write(ADDRESS, 0x02, 0xFF) # chip power down
        i2c.write(ADDRESS, 0x2b, 0x80) # set same lrck
        i2c.write(ADDRESS, 0x00, 0x05) # Play&Record mode
        i2c.write(ADDRESS, 0x01, 0x40) # ibias

        # dac setup
        i2c.write(ADDRESS, 0x04, 0x3C) # Enable Lout / Rout and poweron
        i2c.write(ADDRESS, 0x17, 0x18) # Set i2s and 16bit
        i2c.write(ADDRESS, 0x18, 0x02) # 256xmclk
        i2c.write(ADDRESS, 0x19, 0x00) # unmute codec
        
        # set volume
        i2c.write(ADDRESS, 0x1a, 0x00)
        i2c.write(ADDRESS, 0x1b, 0x00)

        i2c.write(ADDRESS, 0x26, 0x09)
        i2c.write(ADDRESS, 0x27, 0x50)
        i2c.write(ADDRESS, 0x28, 0x38)
        i2c.write(ADDRESS, 0x29, 0x38)
        i2c.write(ADDRESS, 0x2a, 0x50)

        # set volume -45db
        i2c.write(ADDRESS, 0x2e, 32)
        i2c.write(ADDRESS, 0x2f, 32)
        i2c.write(ADDRESS, 0x30, 0x00)
        i2c.write(ADDRESS, 0x31, 0x00)

        # power on
        i2c.write(ADDRESS, 0x02, 0x00)

        i2s_enable_mclk()
        i2s_install(0, 0x01, 16, 44100, true, int(self.getGPIO('bck')), int(self.getGPIO('ws')), int(self.getGPIO('data')), 0)
        
  

        # enable DAC
        gpio_digital_write(21, 1)
        # magic values
        i2c.write(ADDRESS, 0x27, 144)
        i2c.write(ADDRESS, 0x2a, 144)
    end

    def unload_i2s()
        i2s.uninstall()
        i2c.delete()
    end

    def set_volume(volume)
        var realVolume = int((volume / 100.0) * 33)
        i2c.write(0x10, 0x2e, realVolume)
        i2c.write(0x10, 0x2f, realVolume)
    end
end

euphonium.register_driver(ES8388Driver())