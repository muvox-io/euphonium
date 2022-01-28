import math 

class DACDriver
    var hardware_volume_control
    var current_config
    var name
    var signedness

    def get_gpio(pin)
        return int(self.current_config[pin]['value'])
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
        self.config_schema = {
            'dac': {
                'tooltip': 'Select driver',
                'type': 'stringList',
                'listValues': ["I2S", "AC101", "ES8388", "TAS5711", "MA12070P", "Internal"],
                'defaultValue': "I2S"
            },
            'mclk': {
                'tooltip': 'I2S - MCLK',
                'type': 'number',
                'defaultValue': "0"
            },
            'bck': {
                'tooltip': 'I2S - BCK / SCLK',
                'type': 'number',
                'defaultValue': "0"
            },
            'ws': {
                'tooltip': 'I2S - WS / LRCLK',
                'type': 'number',
                'defaultValue': "0"
            },
            'data': {
                'tooltip': 'I2S - DATA / DIN',
                'type': 'number',
                'defaultValue': "0"
            },
            'scl': {
                'tooltip': 'I2C - SCL',
                'type': 'number',
                'defaultValue': "0"
            },
            'sda': {
                'tooltip': 'I2C - SDA',
                'type': 'number',
                'defaultValue': "0"
            },
        }

        self.apply_default_values()

        self.name = "dac"
        self.display_name = "DAC Settings"
        self.type = "system"
        self.selected_driver = nil
        self.is_audio_output = true
        self.registered_drivers = []
    end

    # register driver implementation
    def register_driver(driver)
        self.registered_drivers.push(driver)
        var drivers = []

        for driver : self.registered_drivers
            drivers.push(driver.name)
        end

        self.config_schema['dac']['listValues'] = drivers
    end

    def select_driver(driver_name)
        if (self.selected_driver != nil)
            dac_set_readable(false)
            self.selected_driver.unload_i2s()
        end

        for driver : self.registered_drivers
            if driver.name == driver_name
                self.selected_driver = driver
                self.selected_driver.current_config = self.config_schema
                self.selected_driver.init_i2s()
                dac_set_readable(true)
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
            self.select_driver(self.config_schema['dac']['value'])
        end

        if event == EVENT_VOLUME_UPDATED
            self.selected_driver.set_volume(data)
        end
    end
end

var dac = DACPlugin()

euphonium.register_plugin(dac)
