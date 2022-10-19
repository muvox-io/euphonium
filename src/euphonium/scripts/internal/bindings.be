# TODO this should be done in berry bind directly, however be_class bug prevents implementaton
class Core
    def member(name)
        return get_native('core', name)
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

class WiFi
    def member(name)
        return get_native('wifi', name)
    end
end

class MQTT
    var topic_listeners
    def init()
        self.topic_listeners = {}
    end

    def member(name)
        return get_native('mqtt', name)
    end

    def on_publish(topic, listener)
        self.topic_listeners[topic] = listener
    end
end


# Export APIs
core = Core()
persistor = Persistor()
playback = Playback()
wifi = WiFi()
mqtt = MQTT()