class InternalDriver : DACDriver
    def init()
        self.name = "Internal"
        self.hardwareVolumeControl = false
        setSignedness(true)
    end

    def initI2S()
        internalDAC_install(0, 44100);
    end

    def unloadI2S()
        i2s_delete()
    end

    def setVolume(volume)
    end
end

dac.registerDriver(InternalDriver())
