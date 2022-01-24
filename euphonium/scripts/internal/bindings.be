# TODO this should be done in berry bind directly, however be_class bug prevents implementaton
class Core
    def member(name)
        return get_native('core', name)
    end
end

class I2C
    def member(name)
        return get_native('i2c', name)
    end
end

class Persistor
    def member(name)
        return get_native('persistor', name)
    end
end

class Playback
    def member(name)
        return get_native('playback', name)
    end
end


# Export APIs
core = Core()
i2c = I2C()
persistor = Persistor()
playback = Playback()