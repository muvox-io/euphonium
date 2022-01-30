class AC101Driver : DACDriver
    def init()
        self.name = "AC101"
        self.hardware_volume_control = true
    end

    def init_i2s()
        var ADDRESS = 0x1a

        var config = I2SConfig()
        config.sample_rate = 44100
        config.bits_per_sample = 16
        config.mclk = 0
        config.comm_format = I2S_CHANNEL_FMT_RIGHT_LEFT
        config.channel_format = I2S_COMM_FORMAT_I2S

        i2s.install(config)
        i2s.set_pins(self.get_i2s_pins())
        # setup i2c
        i2c.install(int(self.get_gpio('sda')), int(self.get_gpio('scl')))

        i2c.read_bytes(ADDRESS, 0x00, 2)
        i2c.write_bytes(ADDRESS, 0x00, bytes('0123'))

        sleep_ms(100)

        # enable PLL from BCLK source
        i2c.write_bytes(ADDRESS, 0x01, bytes('014F'))
        i2c.write_bytes(ADDRESS, 0x02, bytes('8600'))

        # clocking system
        i2c.write_bytes(ADDRESS, 0x03, bytes('AA08'))
        i2c.write_bytes(ADDRESS, 0x04, bytes('800C'))
        i2c.write_bytes(ADDRESS, 0x05, bytes('800C'))
        i2c.write_bytes(ADDRESS, 0x06, bytes('7000'))

        # analogue config
        i2c.write_bytes(ADDRESS, 0x10, bytes('8850'))
        i2c.write_bytes(ADDRESS, 0x11, bytes('C000'))
        i2c.write_bytes(ADDRESS, 0x12, bytes('C000'))
        i2c.write_bytes(ADDRESS, 0x13, bytes('2200'))
        i2c.write_bytes(ADDRESS, 0x52, bytes('4440'))

        i2c.write_bytes(ADDRESS, 0x51, bytes('0000'))
        i2c.write_bytes(ADDRESS, 0x40, bytes('0000'))
        i2c.write_bytes(ADDRESS, 0x50, bytes('3300'))

        i2c.write_bytes(ADDRESS, 0x4c, bytes('8800'))
        i2c.write_bytes(ADDRESS, 0x48, bytes('8000'))
        i2c.write_bytes(ADDRESS, 0x53, bytes('F000'))
        i2c.write_bytes(ADDRESS, 0x53, bytes('FF00'))
        i2c.write_bytes(ADDRESS, 0x54, bytes('050A'))

        i2c.write_bytes(ADDRESS, 0x58, bytes('0220')) # speaker out
        i2c.write_bytes(ADDRESS, 0x56, bytes('f801')) # headphones

        # set headphones volume
        self.set_volume(100)

        value = i2c.read_bytes(ADDRESS, 0x02, 2)
		value |= 0x8000
		i2c.write_bytes(ADDRESS, 0x02, bytes().add(value, -2));
    end

    def unload_i2s()
        i2s.uninstall()
        i2c.delete()
    end

    def set_volume(volume)
        var ADDRESS = 0x1a
        var convVolume = (real(volume) / 100) * 255
        var value = int(((convVolume)*0x3f)/255) << 4;
        value |= i2c.read_bytes(ADDRESS, 0x56, 2) & ~(0x3f << 4)
        i2c.write_bytes(ADDRESS, 0x56, bytes().add(value, -2))
    end
end

dac.register_driver(AC101Driver())
