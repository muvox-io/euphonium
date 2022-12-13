# TODO this should be done in berry bind directly, however be_class bug prevents implementaton

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

class WiFi
    def member(name)
        return get_native('wifi', name)
    end
end

# Export APIs
persistor = Persistor()
playback = Playback()
wifi = WiFi()