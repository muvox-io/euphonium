class I2SDriver : DACDriver
    def init()
        self.name = "I2S"
        self.hardwareVolumeControl = false
    end

    def initI2S()
        # 0x01: I2S STAND MODE
        i2s_install(0, 0x01, 16, 44100, true, int(self.getGPIO('bck')), int(self.getGPIO('ws')), int(self.getGPIO('data')), 0)
    end

    def unloadI2S()
        i2s_delete()
        i2c_delete()
    end

    def setDacVolume(volume)
    end

    def setVolume(volume)
    end
end

dac.registerDriver(I2SDriver())
