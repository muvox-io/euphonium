class MA12070P : DACDriver
    var volumeTable
    def init()
        self.name = "MA12070P"
        self.hardwareVolumeControl = true
    end

    def initI2S()
        # All of I2S init logic goes here
    end

    def unloadI2S()
        i2s_delete()
        i2c_delete()
    end

    def setVolume(volume)
        # Volume is in range from 1 to 100
    end
end

dac.registerDriver(MA12070P())
