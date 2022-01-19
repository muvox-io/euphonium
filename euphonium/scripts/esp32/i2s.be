# channel formats
I2S_CHANNEL_FMT_RIGHT_LEFT = 0
I2S_CHANNEL_FMT_ALL_RIGHT = 1
I2S_CHANNEL_FMT_ALL_LEFT = 2
I2S_CHANNEL_FMT_ONLY_RIGHT = 3

# comm formats
I2S_COMM_FORMAT_I2S = 0x01
I2S_COMM_FORMAT_MSB = 0x02
I2S_COMM_FORMAT_PCM_SHORT = 0x04
I2S_COMM_FORMAT_PCM_LONG 0x0C

class I2SConfig
    var sample_rate
    var bits_per_sample
    var channel_format
    var comm_format
    var mclk
end

class I2SWrapper
    # config - instance of I2SConfig
    def install(config)
        _i2s.install(config.sampleRate, config.bitsPerSample, config.channelFormat, config.commFormat, config.enableMclk, config.mclk)
    end

    def uninstall()
        _i2s.uninstall()
    end

    def expand(from, to)
        _i2s.expand(from, to)
    end

    def disable_expand()
        _i2s.disable_expand()
    end

    def set_readable(readable)
        _i2s.set_readable(readable)
    end
end