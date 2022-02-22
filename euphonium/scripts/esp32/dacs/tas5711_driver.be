class TAS5711Driver : DACDriver
    var volume_table
    def init()
        self.name = "TAS5711"
        self.hardware_volume_control = true
    end

    def init_i2s()
        var ADDRESS = 0x1a
        self.volume_table = [255,160,120,100,90,85,80, 75, 70, 65, 61, 57, 53, 50, 47, 44, 41, 38, 35, 32, 29, 26, 23, 20, 17, 14, 12, 10, 8, 6, 4, 2, 0]

        # 0x01: I2S STAND MODE
        var config = I2SConfig()
        config.sample_rate = 44100
        config.bits_per_sample = 16
        config.mclk = 256
        config.comm_format = I2S_CHANNEL_FMT_RIGHT_LEFT
        config.channel_format = I2S_COMM_FORMAT_I2S

        i2s.install(config)
        i2s.set_pins(self.get_i2s_pins())

        i2c.install(int(self.get_gpio('sda')), int(self.get_gpio('scl')))
        gpio.pin_mode(22, gpio.OUTPUT)
        gpio.digital_write(22, 1)

        # enable mclk on GPIO 0
        i2c.write(ADDRESS, 0x1b, 0x00)

        sleep_ms(100)

        i2c.write(ADDRESS, 0x00, 0x6c) # 256 x mlck
        i2c.write(ADDRESS, 0x04, 0x03) # setting, 16 bit i2s
        i2c.write(ADDRESS, 0x05, 0x04) # system control - audioplayback
        i2c.write(ADDRESS, 0x06, 0x00) #disable mute
        i2c.write(ADDRESS, 0x07, 0x50)  # volume
    end

    def unload_i2s()
        i2s.uninstall()
        i2c.delete()
    end

    def set_volume(volume)
        var volume_step = volume / 100.0
        var actual_volume = int(volume_step * 32)
        i2c.write(0x1a, 0x07, self.volume_table[actual_volume])
    end
end

dac.register_driver(TAS5711Driver())