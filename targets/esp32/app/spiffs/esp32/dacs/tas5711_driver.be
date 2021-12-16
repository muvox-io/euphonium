class TAS5711Driver : DACDriver
    def init()
        self.name = "TAS5711"
        self.hardwareVolumeControl = true
    end
end

dac.registerDriver(TAS5711Driver())