class MA12070P : DACDriver
    def init()
        self.name = "MA12070P"
        self.hardware_volume_control = true

    end

    def init_i2s()
        # PINOUT: SDA: 23, SCL: 22, SDATA: 26, LRCLK: 25, BCLK: 5
        # All of I2S init logic goes here
        
        var ADDR = 0x20

        var config = I2SConfig()
        config.sample_rate = 44100
        config.bits_per_sample = 32
        
        # MCLK: 22.58MHz @ 44.1KHz - sufficient for running the dedicated dsp!
        config.mclk = 512       
        config.comm_format = I2S_CHANNEL_FMT_RIGHT_LEFT
        config.channel_format = I2S_COMM_FORMAT_I2S

        i2s.install(config)
        i2s.set_pins(self.get_i2s_pins())

        # Ensures we expand from 16 to 32 bit, to match MA12070P Clock system.
        i2s.expand(16, 32)

        # Start I2C Driver
        i2c.install(int(self.get_gpio('sda')), int(self.get_gpio('scl')))

        # Mute Amplifier before i2c comm & enable. Mute pin: 21
        gpio.pin_mode(8, gpio.OUTPUT)
        gpio.digital_write(8, gpio.LOW)

        # Enable Amplifier. Enable pin: 19
        gpio.pin_mode(19, gpio.OUTPUT)
        gpio.digital_write(19, gpio.LOW)

        # Set Amp to Left-justified format
        i2c.write(ADDR, 53, 8)

        # Set Volume to a safe level..
        i2c.write(ADDR, 64, 0x50)

        # Clear static error register.
        i2c.write(ADDR, 45, 0x34)
        i2c.write(ADDR, 45, 0x30)

        # Init done.

        # Unmute Amplifier 
        gpio.digital_write(8, gpio.HIGH)        
    end

    def unload_i2s()
        i2s.disable_expand()
        i2s.uninstall()
        i2c.delete()
    end

    def set_volume(volume)
        # Volume is in range from 1 to 100
        # Volume register is flipped in MA12070P.. Hence 100 - realvol.
        
        var ADDR = 0x20 
        var realVolume = int(100-volume)

        # Write it..
        i2c.write(ADDR, 64, realVolume)

    end

end

dac.register_driver(MA12070P())

hooks.add_handler(hooks.ON_INIT, def ()

    gpio.pin_mode(27, gpio.OUTPUT)
    gpio.digital_write(27, gpio.HIGH)

end)
