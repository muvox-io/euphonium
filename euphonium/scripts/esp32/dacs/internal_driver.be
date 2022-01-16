class InternalDriver : DACDriver
    def init()
        self.name = "Internal"
        self.hardwareVolumeControl = false
    end

    def initI2S()
        # setSignedness(true)
        internalDAC_install(0, 44100);
    end

    def unloadI2S()
        # setSignedness(false)
        i2s_delete()
    end

    def setVolume(volume)
    end
end

dac.registerDriver(InternalDriver())