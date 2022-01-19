class MA12070P : DACDriver
    var volumeTable
    def init()
        self.name = "MA12070P"
        self.hardwareVolumeControl = false
    end

    def initI2S()
        # PINOUT: SDA: 23, SCL: 22, SDATA: 26, LRCLK: 25, BCLK: 5
        # All of I2S init logic goes here
        var ADDR = 0x10 # IRL, ADDR: 0x20

        # Note the stupid volume mapping. WIP.
        self.volumeTable = [0x60,0x58,0x55,0x52,0x48,0x45,0x42,0x40,0x38,0x35,0x32,0x30,0x28,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25]

        # Ensures we expand from 16 to 32 bit, to match MA12070P Clock system.
        dac_set_expand(16, 32)

        # 0x01 Standard I2S format. Note: MCLK FSx512
        i2s_install(0, 0x01, 32, 44100, true, int(self.getGPIO('bck')), int(self.getGPIO('ws')), int(self.getGPIO('data')), 512)

        # Enable mclk on GPIO 0
        # i2s_enable_mclk()

        # Start I2C Driver
        # i2c_install(true, int(self.getGPIO('sda')), int(self.getGPIO('scl')), 250000) # 250000

        # Mute Amplifier before i2c comm & enable. Mute pin: 12
        # gpio_digital_write(12, 0)

        # Enable Amplifier. Enable pin: 14
        # gpio_digital_write(14, 0)

        # Set Amp to Left-justified format
        i2c_write8(ADDR, 53, 8)

        # Set Volume to a safe level..
        i2c_write8(ADDR, 64, 0x50)

        # Clear static error register.
        # i2c_write8(ADDR, 45, 0x34)
        #i2c_write8(ADDR, 45, 0x30)
        # Init done.
    end

    def unloadI2S()
        i2s_delete()
        #dac_disable_expand()
        i2c_delete()
    end

    def setVolume(volume)
        # Volume is in range from 1 to 100
        # Volume register is flipped in MA12070P.. Hence 128 - realvol.
        var ADDR = 0x10 # IRL, ADDR: 0x20
        var realVolume = int(128-((volume / 100.0) * 33))
        # Write it..
        i2c_write8(ADDR, 64, realVolume)
    end
end

dac.registerDriver(MA12070P())
