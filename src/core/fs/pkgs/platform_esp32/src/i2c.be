class I2SBind
    def member(name)
        return get_native('i2c', name)
    end
end

_i2c = I2SBind()

class I2C
    def install(sda, scl)
        _i2c.master_install(sda, scl, 250000)
    end

    def read(addr, reg)
        return self.read_bytes(addr, reg, 1)
    end

    def write(addr, reg, val)
        return self.write_bytes(addr, reg, bytes().add(val))
    end

    def delete()
        _i2c.delete()
    end

    def detect(addr)
        return _i2c.detect(addr)
    end

    def write_bytes(addr, reg, val)
        return self.write_raw(addr, bytes().add(reg)..val)
    end

    def read_bytes(addr, reg, size)
        return self.read_raw(addr, bytes().add(reg), size)
    end

    def write_raw(addr, val)
        var intResult = list()
        for i : 0..(val.size()-1)
            intResult.push(val[i])
        end

        _i2c.master_write_to_device(0, addr, intResult)
    end

    def read_raw(addr, val, size)
        var intResult = list()
        for i : 0..(val.size()-1)
            intResult.push(val[i])
        end

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
