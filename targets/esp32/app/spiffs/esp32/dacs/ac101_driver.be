class AC101Driver : DACDriver
    def init()
        self.name = "AC101"
        self.hardwareVolumeControl = true
    end

    def initI2S()
        var ADDRESS = 0x1a

        # 0x01: I2S STAND MODE
        i2s_install(0, 0x01, 44100, true, int(self.getGPIO('bck')), int(self.getGPIO('ws')), int(self.getGPIO('data')), 0)

        # setup i2c
        i2c_install(true, int(self.getGPIO('sda')), int(self.getGPIO('scl')), 250000)

        i2c_read16(ADDRESS, 0x00)
        i2c_write16(ADDRESS, 0x00, 0x123)

        sleep_ms(100)

        # enable PLL from BCLK source
        i2c_write16(ADDRESS, 0x01, 0x14F)
        i2c_write16(ADDRESS, 0x02, 0x8600)

        # clocking system
        i2c_write16(ADDRESS, 0x03, 0xAA08)
        i2c_write16(ADDRESS, 0x04, self.BIN('1000000000001100'))
        i2c_write16(ADDRESS, 0x05, self.BIN('1000000000001100'))
        i2c_write16(ADDRESS, 0x06, self.BIN('0111000000000000'))

        # analogue config
        i2c_write16(ADDRESS, 0x10, self.BIN('1000100001010000'))
        i2c_write16(ADDRESS, 0x11, self.BIN('1100000000000000'))
        i2c_write16(ADDRESS, 0x12, self.BIN('1100000000000000'))
        i2c_write16(ADDRESS, 0x13, self.BIN('0010001000000000'))
        i2c_write16(ADDRESS, 0x52, self.BIN('0100010001000000'))

        i2c_write16(ADDRESS, 0x51, 0)
        i2c_write16(ADDRESS, 0x40, 0)
        i2c_write16(ADDRESS, 0x50, self.BIN('0011001100000000'))

        i2c_write16(ADDRESS, 0x4c, self.BIN('1000100000000000'))
        i2c_write16(ADDRESS, 0x48, self.BIN('1000000000000000'))
        i2c_write16(ADDRESS, 0x53, self.BIN('1111000000000000'))
        i2c_write16(ADDRESS, 0x53, self.BIN('1111111100000000'))
        i2c_write16(ADDRESS, 0x54, self.BIN('0000010100001010'))

        i2c_write16(ADDRESS, 0x58, 0x0220) # speaker out
        i2c_write16(ADDRESS, 0x56, 0xf801) # headphones

        # set headphones volume
        self.setDacVolume(100)

        value = i2c_read16(ADDRESS, 0x02)
		value |= 0x8000
		i2c_write16(ADDRESS, 0x02, value);
    end

    def unloadI2S()
        i2s_delete()
        i2c_delete()
    end

    def setDacVolume(volume)
        var ADDRESS = 0x1a
        var convVolume = (real(volume) / 100) * 255
        var value = int(((convVolume)*0x3f)/255) << 4;
        value |= i2c_read16(ADDRESS, 0x56) & ~(0x3f << 4)
        i2c_write16(ADDRESS, 0x56, value)
    end
end

dac.registerDriver(AC101Driver())
