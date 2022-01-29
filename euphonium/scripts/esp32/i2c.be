class I2SBind
    def member(name)
        return get_native('i2c', name)
    end
end

_i2c = I2SBind()

class I2C
    def install(sda, scl)
        i2c_install(true, sda, scl, 250000)
        _i2c.master_init()
    end

    def read()
    end

    def write()
    end

    def write_bytes(addr, reg, val)
        var intResult = list()
        intResult.push(reg)
        for i : 0..(val.size()-1)
            intResult.push(val[i])
        end

        _i2c.master_write_to_device(0, addr, intResult)
    end

    def read_bytes(addr, reg, size)
        var intResult = list()
        intResult.push(reg)
        var res = _i2c.master_write_read_device(0, addr, intResult, size)

        var resultBytes = bytes()
        resultBytes.resize(res.size())
        for i : 0..(res.size()-1)
            resultBytes[i] = res[i]
        end

        return resultBytes.get(0, -size)
    end
end

i2c = I2C()