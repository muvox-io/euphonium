class DummyDriver : DACDriver
    def init()
        self.name = "dummy"
        self.hardware_volume_control = true
    end

    def init_i2s()
       log_info("INITIALIZING DUMMY DRIVER - NO AUDIO")
    end

    def unload_i2s()
       
    end

    def set_volume(volume)
     
    end

    def make_config_form(ctx, state)
    end
end

hardware.register_driver(DummyDriver())
