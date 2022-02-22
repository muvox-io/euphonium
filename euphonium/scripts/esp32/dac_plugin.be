import math 

class DACDriver
    var hardware_volume_control
    var state
    var name
    var signedness

    def get_gpio(pin)
        return int(self.state[pin])
    end

    def get_i2s_pins()
        var pins_config = I2SPinsConfig()
        pins_config.mck_io = 0
        pins_config.bck_io = self.get_gpio('bck')
        pins_config.ws_io = self.get_gpio('ws')
        pins_config.data_out_io = self.get_gpio('data')

        return pins_config
    end

    # initializes the DAC driver
    def init_i2s()
    end

    # disables I2S
    def unload_i2s()
    end

    # sets the volume of the DAC
    def set_volume(volume)
    end

    def bin(num)
        var res = 0
        for x : 1..size(num)
            res += math.pow(2, x-1) * int(num[size(num) - x])
        end
        return int(res)
    end    
end

class DACPlugin : Plugin
    var registered_drivers
    var selected_driver

    def init()
        self.registered_drivers = []
        self.apply_default_values()

        self.name = "dac"
        self.display_name = "DAC Settings"
        self.type = "system"
        self.selected_driver = nil
        self.is_audio_output = true
    end

    def make_form(ctx, state)
        var drivers = []

        for driver : self.registered_drivers
            drivers.push(driver.name)
        end

        ctx.create_group('driver', { 'label': 'Driver' })
        ctx.create_group('i2s', { 'label': 'I2S GPIO' })
        ctx.create_group('i2c', { 'label': 'I2C GPIO' })

        ctx.select_field('dac', {
            'label': "Select driver",
            'default': "I2S",
            'group': 'driver',
            'values': drivers,
            'type': 'number'
        })

        ctx.number_field('bck', {
            'label': "BCK",
            'default': "0",
            'group': 'i2s',
        })

        ctx.number_field('ws', {
            'label': "WS",
            'default': "0",
            'group': 'i2s',
        })

        ctx.number_field('data', {
            'label': "DATA",
            'default': "0",
            'group': 'i2s',
        })

        ctx.number_field('mclk', {
            'label': "MCLK",
            'default': "0",
            'group': 'i2s',
        })

        ctx.number_field('sda', {
            'label': "SDA",
            'default': "0",
            'group': 'i2c',
        })

        ctx.number_field('scl', {
            'label': "SCL",
            'default': "0",
            'group': 'i2c',
        })
    end

    # register driver implementation
    def register_driver(driver)
        self.registered_drivers.push(driver)
    end

    def select_driver(driver_name)
        if (self.selected_driver != nil)
            i2s.set_readable(false)
            self.selected_driver.unload_i2s()
        end

        for driver : self.registered_drivers
            if driver.name == driver_name
                self.selected_driver = driver
                self.selected_driver.state = self.state
                self.selected_driver.init_i2s()
                i2s.set_readable(true)
            end
        end
    end

    def init_audio()
    end

    def has_hardware_volume()
        return self.selected_driver.hardware_volume_control
    end

    def on_event(event, data)
        if event == EVENT_CONFIG_UPDATED
            self.select_driver(self.state['dac'])
        end

        if event == EVENT_VOLUME_UPDATED
            self.selected_driver.set_volume(data)
        end
    end
end

var dac = DACPlugin()

euphonium.register_plugin(dac)
