class TAS5711Driver : DACDriver
    var volumeTable
    def init()
        self.name = "TAS5711"
        self.hardwareVolumeControl = true
    end

    def initI2S()
        var ADDRESS = 0x1a
        self.volumeTable = [255,160,120,100,90,85,80, 75, 70, 65, 61, 57, 53, 50, 47, 44, 41, 38, 35, 32, 29, 26, 23, 20, 17, 14, 12, 10, 8, 6, 4, 2, 0]

        # 0x01: I2S STAND MODE
        i2s_install(0, 0x01, 44100, true, int(self.getGPIO('bck')), int(self.getGPIO('ws')), int(self.getGPIO('data')), 256)

        i2c_install(true, int(self.getGPIO('sda')), int(self.getGPIO('scl')), 250000)
        # enable mclk on GPIO 0
        i2s_enable_mclk()

        i2c_write8_val(ADDRESS, 0x00)
        i2c_read8(ADDRESS, 0x00)

        sleep_ms(100)

        i2c_write8(ADDRESS, 0x1b, 0x00)

        sleep_ms(100)

        i2c_write8(ADDRESS, 0x00, 0x6c) # 256 x mlck
        i2c_write8(ADDRESS, 0x04, 0x03) # setting, 16 bit i2s
        i2c_write8(ADDRESS, 0x05, 0x04) # system control - audioplayback
        i2c_write8(ADDRESS, 0x06, 0x00) #disable mute
        i2c_write8(ADDRESS, 0x07, 0x50)  # volume
    end

    def unloadI2S()
        i2s_delete()
        i2c_delete()
    end

    def setVolume(volume)
        var volumeStep = volume / 100.0
        var actualVolume = int(volumeStep * 32)
        i2c_write8(0x1a, 0x07, self.volumeTable[actualVolume])
    end
end

dac.registerDriver(TAS5711Driver())
