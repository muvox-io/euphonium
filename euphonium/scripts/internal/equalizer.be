EQUALIZER_LOW_SHELF_FREQUENCY  = 250  # Hz
EQUALIZER_NOTCH_FREQUENCY      = 1000 # Hz
EQUALIZER_HIGH_SHELF_FREQUENCY = 4000 # Hz

Q_FACTOR       = 0.7071 # sqrt(2) / 2 // 0.7071067812
EQUALIZER_GAIN = 3     // dB

class Equalizer
    var bass
    var mid
    var treble
    
    var bassFilters
    var midFilters
    var trebleFilters

    def init()
        defBassParams   = { 'f': EQUALIZER_LOW_SHELF_FREQUENCY,  'g': 0.0, q: Q_FACTOR     }
        defMidParams    = { 'f': EQUALIZER_NOTCH_FREQUENCY,      'g': 0.0, q: Q_FACTOR / 2 }
        defTrebleParams = { 'f': EQUALIZER_HIGH_SHELF_FREQUENCY, 'g': 0.0, q: Q_FACTOR     }

        self.bassFilters = [
            BiquadFilter(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_LOWSHELF, defBassParams), 
            BiquadFilter(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_LOWSHELF, defBassParams), 
            BiquadFilter(dsp.CHANNEL_BOTH,  dsp.BIQUAD_TYPE_LOWSHELF, defBassParams)
        ]

        self.midFilters = [
            BiquadFilter(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_NOTCH, defMidParams), 
            BiquadFilter(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_NOTCH, defMidParams), 
            BiquadFilter(dsp.CHANNEL_BOTH,  dsp.BIQUAD_TYPE_NOTCH, defMidParams)
        ]

        self.trebleFilters = [
            BiquadFilter(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_HIGHSHELF, defTrebleParams), 
            BiquadFilter(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_HIGHSHELF, defTrebleParams), 
            BiquadFilter(dsp.CHANNEL_BOTH,  dsp.BIQUAD_TYPE_HIGHSHELF, defTrebleParams)
        ]
    end

    def set_chain()
        dsp.clear_chain()
    end

    def set_eq(bass, mid, treble)
        for filter : self.bassFilters
            filter.edit(dsp.BIQUAD_TYPE_LOWSHELF, { 'f': EQUALIZER_LOW_SHELF_FREQUENCY,  'g': bass * EQUALIZER_GAIN, q: Q_FACTOR})
        end

        for filter : self.midFilters
            filter.edit(dsp.BIQUAD_TYPE_NOTCH, { 'f': EQUALIZER_NOTCH_FREQUENCY,  'g': bass * EQUALIZER_GAIN, q: Q_FACTOR / 2 })
        end

        for filter : self.trebleFilters
            filter.edit(dsp.BIQUAD_TYPE_HIGHSHELF, { 'f': EQUALIZER_HIGH_SHELF_FREQUENCY,  'g': bass * EQUALIZER_GAIN, q: Q_FACTOR})
        end
    end
end

eq = Equalizer()
