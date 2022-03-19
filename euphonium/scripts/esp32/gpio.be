class GPIO
    var LOW
    var HIGH
    var INPUT
    var INPUT_PULLUP
    var INPUT_PULLDOWN
    var OUTPUT
    var BUTTON_PRESS
    var LONG_PRESS
    var DOUBLE_PRESS

    var button_handler_map

    def init()
        self.button_handler_map = {}

        euphonium.register_handler('buttonInteractionEvent', def (interaction)
            if (self.button_handler_map.find(interaction['button']) != nil && self.button_handler_map[interaction['button']]['type'] == interaction['type']) 
                self.button_handler_map[interaction['button']]['handler']()
            end
        end)
    end

    def member(name)
        return get_native('gpio', name)
    end

    def register_button(button, event_type, callback, config)
        register_button_native = get_native('gpio', 'register_button')
        self.button_handler_map[button] = {
            'type': event_type,
            'handler': callback
        }

        var default_high_state = false
        if (config != nil) 
            if (config.find('high_state') != nil)
                default_high_state = config['high_state']
            end
        end

        register_button_native(button, default_high_state)
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

#- Button actions -#
gpio.PRESS = 'PRESS'
gpio.DOUBLE_PRESS = 'DOUBLE_PRESS'
gpio.LONG_PRESS = 'LONG_PRESS'