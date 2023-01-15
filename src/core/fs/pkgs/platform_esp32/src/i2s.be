# channel formats
I2S_CHANNEL_FMT_RIGHT_LEFT = 0
I2S_CHANNEL_FMT_ALL_RIGHT = 1
I2S_CHANNEL_FMT_ALL_LEFT = 2
I2S_CHANNEL_FMT_ONLY_RIGHT = 3

# comm formats
I2S_COMM_FORMAT_I2S = 0x01
I2S_COMM_FORMAT_MSB = 0x02
I2S_COMM_FORMAT_PCM_SHORT = 0x04
I2S_COMM_FORMAT_PCM_LONG = 0x0C

class I2SBind
    def member(name)
        return get_native('i2s', name)
    end
end

_i2s = I2SBind()

class I2SConfig
    var sample_rate
    var bits_per_sample
    var channel_format
    var comm_format
    var mclk
end

class I2SPinsConfig
    var mck_io
    var bck_io
    var ws_io
    var data_out_io
end

class I2SWrapper
    # config - instance of I2SConfig
    def install(config)
        _i2s.install(int(config.sample_rate), int(config.bits_per_sample), int(config.channel_format), int(config.comm_format), int(config.mclk))
    end

    def uninstall()
        _i2s.uninstall()
    end

    # config - instance of I2SPinsConfig
    def set_pins(config)
        _i2s.set_pins(config.mck_io, config.bck_io, config.ws_io, config.data_out_io)
    end

    def expand(from, to)
        _i2s.set_expand(from, to)
    end

    def disable_expand()
        _i2s.set_expand(16, 16)
    end

    def set_readable(readable)
        _i2s.set_readable(readable)
    end
end

i2s = I2SWrapper()
