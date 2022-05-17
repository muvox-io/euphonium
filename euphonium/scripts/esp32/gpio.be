class GPIO
    var LOW
    var HIGH
    var INPUT
    var INPUT_PULLUP
    var INPUT_PULLDOWN
    var OUTPUT

    var PRESS
    var LONG_PRESS
    var DOUBLE_PRESS
    var ENCODER_CW
    var ENCODER_CCW

    var button_handler_map
    var encoder_handler_list

    def init()
        self.button_handler_map = {}
        self.encoder_handler_list = []

        euphonium.register_handler('buttonInteractionEvent', def (interaction)
            if (self.button_handler_map.find(interaction['button']) != nil && self.button_handler_map[interaction['button']]['type'] == interaction['type']) 
                self.button_handler_map[interaction['button']]['handler']()
            end

            # ENCODER INTERACTION
            if (interaction['type'] == self.ENCODER_CW || interaction['type'] == self.ENCODER_CCW)
                for handler : self.encoder_handler_list
                    if handler['pinA'] == interaction['button'] || handler['pinB'] == interaction['button']
                        handler['callback'](interaction['type'])
                    end
                end
            end
        end)
    end

    def member(name)
        return get_native('gpio', name)
    end

    def register_button(button, event_type, callback)
        register_button_native = get_native('gpio', 'register_button')
        self.button_handler_map[button] = {
            'type': event_type,
            'handler': callback
        }

        register_button_native(button)
    end

    def register_encoder(pinA, pinB, callback)
        register_encoder_native = get_native('gpio', 'register_encoder')
        self.encoder_handler_list.push(
            {
                'pinA': pinA,
                'pinB': pinB,
                'callback': callback
            }
        )
        register_encoder_native(pinA, pinB)
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
gpio.ENCODER_CW = 'encoder_cw'
gpio.ENCODER_CCW = 'encoder_ccw'