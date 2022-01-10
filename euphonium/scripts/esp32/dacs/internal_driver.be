class InternalDriver : DACDriver
    def init()
        self.name = "Internal"
        self.hardwareVolumeControl = false
        self.signedness = true
    end

    def initI2S()

    end

    def unloadI2S()

    end

    def setVolume(volume)
    end
end

dac.registerDriver(InternalDriver())
