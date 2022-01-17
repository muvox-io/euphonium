class ES8388Driver : DACDriver
    def init()
        self.name = "ES8388"
        self.hardwareVolumeControl = true
    end

    def initI2S()
        var ADDRESS = 0x10

        # 0x01: I2S STAND MODE

        # setup i2c
        i2c_install(true, int(self.getGPIO('sda')), int(self.getGPIO('scl')), 100000)
        sleep_ms(50)
        
        # init
        i2c_write8(ADDRESS, 0x08, 0x00) # set slave mode
        i2c_write8(ADDRESS, 0x02, 0xFF) # chip power down
        i2c_write8(ADDRESS, 0x2b, 0x80) # set same lrck
        i2c_write8(ADDRESS, 0x00, 0x05) # Play&Record mode
        i2c_write8(ADDRESS, 0x01, 0x40) # ibias

        # dac setup
        i2c_write8(ADDRESS, 0x04, 0x3C) # Enable Lout / Rout and poweron
        i2c_write8(ADDRESS, 0x17, 0x18) # Set i2s and 16bit
        i2c_write8(ADDRESS, 0x18, 0x02) # 256xmclk
        i2c_write8(ADDRESS, 0x19, 0x00) # unmute codec
        
        # set volume
        i2c_write8(ADDRESS, 0x1a, 0x00)
        i2c_write8(ADDRESS, 0x1b, 0x00)

        i2c_write8(ADDRESS, 0x26, 0x09)
        i2c_write8(ADDRESS, 0x27, 0x50)
        i2c_write8(ADDRESS, 0x28, 0x38)
        i2c_write8(ADDRESS, 0x29, 0x38)
        i2c_write8(ADDRESS, 0x2a, 0x50)

        # set volume -45db
        i2c_write8(ADDRESS, 0x2e, 32)
        i2c_write8(ADDRESS, 0x2f, 32)
        i2c_write8(ADDRESS, 0x30, 0x00)
        i2c_write8(ADDRESS, 0x31, 0x00)

        # power on
        i2c_write8(ADDRESS, 0x02, 0x00)

        i2s_enable_mclk()
        i2s_install(0, 0x01, 16, 44100, true, int(self.getGPIO('bck')), int(self.getGPIO('ws')), int(self.getGPIO('data')), 0)
        
  

        # enable DAC
        gpio_digital_write(21, 1)
        # magic values
        i2c_write8(ADDRESS, 0x27, 144)
        i2c_write8(ADDRESS, 0x2a, 144)
    end

    def unloadI2S()
        i2s_delete()
        i2c_delete()
    end

    def setVolume(volume)
        var realVolume = int((volume / 100.0) * 33)
        i2c_write8(0x10, 0x2e, realVolume)
        i2c_write8(0x10, 0x2f, realVolume)
    end
end

dac.registerDriver(ES8388Driver())