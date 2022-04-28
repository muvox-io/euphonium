class JP2137 : DACDriver
    var volume_table
    def init()
        # define a volume table, saves up on log10
        self.volume_table = [255,160,120,100,90,85,80, 75, 70, 65, 61, 57, 53, 50, 47, 44, 41, 38, 35, 32, 29, 26, 23, 20, 17, 14, 12, 10, 8, 6, 4, 2, 0]
        self.name = "JP2137"
        self.hardware_volume_control = true
        self.datasheet_link = "https://www.infineon.com/dgdl/Infineon-MA12070P-DS-v01_00-EN.pdf?fileId=5546d46264a8de7e0164b761f2f261e4"
    end

    def init_i2s()
        test_strip = LEDStrip(0, 4, 12, 0, 10)
        test_strip[0] = [255, 0, 0]
        test_strip.show()
        # PINOUT: SDA: 23, SCL: 22, SDATA: 26, LRCLK: 25, BCLK: 5
        # All of I2S init logic goes here
        var ADDR = 0x20

        var config = I2SConfig()
        config.sample_rate = 44100
        config.bits_per_sample = 32
        
        # MCLK: 22.58MHz @ 44.1KHz - sufficient for running the dedicated dsp!
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
        gpio.pin_mode(21, gpio.OUTPUT)
        # Enable Amplifier. Enable pin: 19
        gpio.pin_mode(19, gpio.OUTPUT)

        gpio.digital_write(19, gpio.HIGH)       
        gpio.digital_write(21, gpio.HIGH) 
    end

    def unload_i2s()
        i2s.disable_expand()
        i2s.uninstall()
        i2c.delete()
    end

    def set_volume(volume)
        # Volume is in range from 1 to 100
        # Volume register is flipped in MA12070P.. Hence 100 - realvol.
        var volume_step = volume / 100.0
        var actual_volume = int(volume_step * 32)

        var ADDR = 0x20
        var current_volume = self.volume_table[actual_volume]
        if (current_volume < 32)
            current_volume = 32
        end
        # Write it..
        i2c.write_raw(ADDR, bytes().add(0x00).add(0x03).add(current_volume))

    end

    def make_config_form(ctx, state)
        ctx.create_group('JP2137_pins', { 'label': 'DAC binary pins' })
        
        ctx.number_field('enablePin', {
            'label': "Enable Pin",
            'default': "0",
            'group': 'JP2137_pins',
        })

        ctx.number_field('mutePin', {
            'label': "Mute Pin",
            'default': "0",
            'group': 'JP2137_pins',
        })
        super(self).make_config_form(ctx, state)
    end

end

gpio.register_encoder(2, 18, def (state) 
    print("Encoder state")
    print(state)
end)



hardware.register_driver(JP2137())