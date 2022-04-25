# Rmt channels
RMT_CHANNEL_0 = 0
RMT_CHANNEL_1 = 1
RMT_CHANNEL_2 = 1
RMT_CHANNEL_3 = 3
RMT_CHANNEL_4 = 4
RMT_CHANNEL_5 = 5
RMT_CHANNEL_6 = 6
RMT_CHANNEL_7 = 7

# LED Types
LED_WS2812 = 1
LED_WS2812B = 2
LED_SK6812 = 3
LED_WS2813 = 4

class LEDStrip
    var channel
    var length
    var brightness
    def init(type, pin, len, chan, brightness)
        self.channel = chan
        self.length = len
        if (brightness != nil)
            self.brightness = brightness
        else
            self.brightness = 255
        end

        get_native('led', 'create_strip')(self.channel, 0, self.length, pin, self.brightness)
    end
    

    def setitem(index, item)
        get_native('led', 'set_pixel_rgb')(self.channel, index, item[0], item[1], item[2])
    end

    def show()
        get_native('led', 'show')(self.channel)
    end
end

test_strip = LEDStrip(0, 4, 12, 0, 10)

euphonium.on_event(EVENT_VOLUME_UPDATED, def (volume) 
    leds_to_show = int((real(volume) /100) * 12)
    step = ((real(volume) /100) * 12) - leds_to_show
    for led_index : 0..(leds_to_show-1)
        test_strip[led_index] = [255, 255, 255]
    end

    for led_index : leds_to_show..11
        test_strip[led_index] = [0, 0, 0]
    end

    test_strip[leds_to_show] = [int(255 * step), int(255 * step), int(255 * step)]

    test_strip.show()
end)