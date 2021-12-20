class TAS5711Driver : DACDriver
    def init()
        self.name = "TAS5711"
        self.hardwareVolumeControl = true
    end

    def initI2S()
        var ADDRESS = 0x1a

        # 0x01: I2S STAND MODE
        i2s_install(0, 0x01, 44100, true, int(self.getGPIO('bck')), int(self.getGPIO('ws')), int(self.getGPIO('data')))

        # setup i2c
        i2c_install(true, int(self.getGPIO('sda')), int(self.getGPIO('scl')), 250000)
    end
end

dac.registerDriver(TAS5711Driver())
