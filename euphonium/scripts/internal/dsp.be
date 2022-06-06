class DSPProcessor
    # Channels
    var CHANNEL_LEFT
    var CHANNEL_RIGHT

    # Biquad types
    var BIQUAD_TYPE_LOWPASS
    var BIQUAD_TYPE_HIGHPASS
    var BIQUAD_TYPE_NOTCH
    var BIQUAD_TYPE_PEAK
    var BIQUAD_TYPE_LOWSHELF
    var BIQUAD_TYPE_HIGHSHELF

    def init()
        self.CHANNEL_LEFT = 0
        self.CHANNEL_RIGHT = 1

        self.BIQUAD_TYPE_HIGHPASS = 0
        self.BIQUAD_TYPE_LOWPASS = 1
        self.BIQUAD_TYPE_NOTCH = 2
        self.BIQUAD_TYPE_PEAK = 3
        self.BIQUAD_TYPE_LOWSHELF = 4
        self.BIQUAD_TYPE_HIGHSHELF = 5
    end

    def clear_chain()
        get_native('dsp', 'clear_chain')()
    end

    def add_mono_downmix()
        get_native('dsp', 'set_mono_downmix')(true)
    end

    def add_biquad(channel, type, parameters)
        if (parameters.find('g') == nil)
            parameters['g'] = 0.0
        end

        get_native('dsp', 'register_biquad')(channel, type, real(parameters['f']), real(parameters['g']), real(parameters['q']))
    end
end

dsp = DSPProcessor()