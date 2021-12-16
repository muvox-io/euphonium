class I2SDriver : DACDriver
    def init()
        self.name = "I2S"
        self.hardwareVolumeControl = false
    end

    def initI2S()
    end

    def unloadI2S()
    end

    def setVolume(volume)
    end
end

dac.registerDriver(I2SDriver())