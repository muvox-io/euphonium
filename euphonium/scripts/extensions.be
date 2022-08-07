# # Here add your own initialiaization files

dsp.add_mono_downmix()

# Crossover
dsp.add_biquad(dsp.CHANNEL_LEFT, dsp.BIQUAD_TYPE_LOWPASS, { 'f': real(0.079365079365079), 'q': 0.7 })

# Protection filter
## Not sure about the Q for a 8th order filter, so adding several 2nd ones instead
dsp.add_biquad(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_HIGHPASS, { 'f': real(0.001179138321995), 'q': 0.7 })
dsp.add_biquad(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_HIGHPASS, { 'f': real(0.001179138321995), 'q': 0.7 })
dsp.add_biquad(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_HIGHPASS, { 'f': real(0.001179138321995), 'q': 0.7 })
dsp.add_biquad(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_HIGHPASS, { 'f': real(0.001179138321995), 'q': 0.7 })
#dsp.add_biquad(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_HIGHPASS, { 'f': real(0.001179138321995), 'q': 0.7 })
#dsp.add_biquad(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_HIGHPASS, { 'f': real(0.001179138321995), 'q': 0.7 })
#dsp.add_biquad(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_HIGHPASS, { 'f': real(0.001179138321995), 'q': 0.7 })
#dsp.add_biquad(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_HIGHPASS, { 'f': real(0.001179138321995), 'q': 0.7 })

# Vocing
#HS_1_negative
dsp.add_biquad(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_HIGHSHELF, { 'f': real(0.002267573696145), 'g': -10, 'q': 0.7 })
dsp.add_biquad(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_HIGHSHELF, { 'f': real(0.002267573696145), 'g': -10, 'q': 0.7 })
#HS_2_positive
dsp.add_biquad(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_HIGHSHELF, { 'f': real(0.040816326530612), 'g': 9, 'q': 0.7 })
dsp.add_biquad(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_HIGHSHELF, { 'f': real(0.040816326530612), 'g': 9, 'q': 0.7 })
#HS_2_negative
dsp.add_biquad(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_HIGHSHELF, { 'f': real(0.073242630385488), 'g': -4, 'q': 0.7 })
dsp.add_biquad(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_HIGHSHELF, { 'f': real(0.073242630385488), 'g': -4, 'q': 0.7 })
#HS_3_negative
dsp.add_biquad(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_HIGHSHELF, { 'f': real(0.007936507936508), 'g': -0.9, 'q': 0.7 })
dsp.add_biquad(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_HIGHSHELF, { 'f': real(0.007936507936508), 'g': -0.9, 'q': 0.7 })

#peak_1
dsp.add_biquad(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_PEAKING_EQ, { 'f': real(0.001768707482993), 'g': 10, 'q': 3 })
dsp.add_biquad(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_PEAKING_EQ, { 'f': real(0.001768707482993), 'g': 10, 'q': 3 })
#peak_2
dsp.add_biquad(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_PEAKING_EQ, { 'f': real(0.004195011337868), 'g': -3.8, 'q': 2 })
dsp.add_biquad(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_PEAKING_EQ, { 'f': real(0.004195011337868), 'g': -3.8, 'q': 2 })
#peak_3
dsp.add_biquad(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_PEAKING_EQ, { 'f': real(0.014739229024943), 'g': 4, 'q': 6 })
dsp.add_biquad(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_PEAKING_EQ, { 'f': real(0.014739229024943), 'g': 4, 'q': 6 })
#peak_4
dsp.add_biquad(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_PEAKING_EQ, { 'f': real(0.011995464852608), 'g': -3.5, 'q': 6 })
dsp.add_biquad(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_PEAKING_EQ, { 'f': real(0.011995464852608), 'g': -3.5, 'q': 6 })
#peak_5
dsp.add_biquad(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_PEAKING_EQ, { 'f': real(0.032086167800454), 'g': -2, 'q': 2.5 })
dsp.add_biquad(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_PEAKING_EQ, { 'f': real(0.032086167800454), 'g': -2, 'q': 2.5 })
#peak_6
dsp.add_biquad(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_PEAKING_EQ, { 'f': real(0.001360544217687), 'g': 6, 'q': 5 })
dsp.add_biquad(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_PEAKING_EQ, { 'f': real(0.001360544217687), 'g': 6, 'q': 5 })
#peak_7
dsp.add_biquad(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_PEAKING_EQ, { 'f': real(0.072562358276644), 'g': 3, 'q': 5 })
dsp.add_biquad(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_PEAKING_EQ, { 'f': real(0.072562358276644), 'g': 3, 'q': 5 })
#peak_8
dsp.add_biquad(dsp.CHANNEL_RIGHT, dsp.BIQUAD_TYPE_PEAKING_EQ, { 'f': real(0.149659863945578), 'g': 3, 'q': 3 })
dsp.add_biquad(dsp.CHANNEL_LEFT,  dsp.BIQUAD_TYPE_PEAKING_EQ, { 'f': real(0.149659863945578), 'g': 3, 'q': 3 })

dsp.set_gain_db(-9)
