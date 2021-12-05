import math 

class DACDriver
    var hardwareVolumeControl
    var currentConfig
    var name

    def getGPIO(pin)
        return int(self.currentConfig[pin]['value'])
    end

    # initializes the DAC driver
    def initI2S()
    end

    # disables I2S
    def unloadI2S()
    end

    # sets the volume of the DAC
    def setVolume(volume)
    end

    def BIN(num)
        var res = 0
        for x : 1..size(num)
            res += math.pow(2, x-1) * int(num[size(num) - x])
        end
        return int(res)
    end    
end

class DACPlugin : Plugin
    var registeredDrivers
    var selectedDriver

    def init()
        self.configSchema = {
            'driver': {
                'tooltip': 'Select driver',
                'type': 'stringList',
                'listValues': ["I2S", "AC101", "TAS5711"],
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
                'tooltip': 'I2S - SDA',
                'type': 'number',
                'defaultValue': "0"
            },
        }

        self.applyDefaultValues()

        self.name = "dac"
        self.displayName = "DAC Settings"
        self.type = "system"
        self.selectedDriver = nil
        self.audioOutput = true
        self.registeredDrivers = []
    end

    # register driver implementation
    def registerDriver(driver)
        self.registeredDrivers.push(driver)
        var drivers = []

        for driver : self.registeredDrivers
            drivers.push(driver.name)
        end

        self.configSchema['driver']['listValues'] = drivers
    end

    def selectDriver(driverName)
        if (self.selectedDriver != nil)
            dac_set_readable(false)
            self.selectedDriver.unloadI2S()
        end

        for driver : self.registeredDrivers
            if driver.name == driverName
                self.selectedDriver = driver
                self.selectedDriver.currentConfig = self.configSchema
                self.selectedDriver.initI2S()
                dac_set_readable(true)
            end
        end
    end

    def initAudio()
    end

    def configurationUpdated()
        self.selectDriver(self.configSchema['driver']['value'])
    end

    def volumeChanged(volume)
        self.selectedDriver.setVolume(volume)
    end
end

var dac = DACPlugin()

app.registerPlugin(dac)
