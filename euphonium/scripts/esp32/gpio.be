class GPIO
    var LOW
    var HIGH
    var INPUT
    var INPUT_PULLUP
    var INPUT_PULLDOWN
    var OUTPUT

    def member(name)
        return get_native('i2s', name)
    end
end

gpio = GPIO()

#- HIGH/LOW -#
gpio.LOW = 0
gpio.HIGH = 1

#- GPIO states -#
gpio.INPUT = 1
gpio.INPUT_PULLUP = 2
gpio.INPUT_PULLDOWN = 3
gpio.OUTPUT = 4