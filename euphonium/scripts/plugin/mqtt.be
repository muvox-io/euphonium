class MQTTPlugin : Plugin
    def init()
        self.apply_default_values()
        self.name = "mqtt"
        self.theme_color = "#d2c464"
        self.display_name = "MQTT"
        self.type = "plugin"

        mqtt.connect("192.168.1.19", "1883", "mqtt", "1g1Kc817G7Tif3zP")

        mqtt.publish("homeassistant/number/euphonium_volume/config", json.dump({
            'name': 'Euphonium volume',
            'command_topic': 'euphonium/volume/set',
            'state_topic': 'euphonium/volume/state'
        }))
        
        mqtt.subscribe("euphonium/volume/set")
        mqtt.on_publish("euphonium/volume/set", def (volume) 
            euphonium.apply_volume(int(volume))
        end)
    end

    def make_form(ctx, state)
        ctx.create_group('mqtt', { 'label': 'General' })

        ctx.text_field('mqttBrokerUrl', {
            'label': "MQTT Broker address",
            'default': "",
            'group': 'mqtt'
        })
    end


    def on_event(event, data)
        if event == EVENT_VOLUME_UPDATED
            mqtt.publish('euphonium/volume/state', str(data))
        end
    end
end

euphonium.register_plugin(MQTTPlugin())

# connect the publish event with topic listeners on mqtt object
euphonium.register_handler("mqttPublishEvent", def (event)
    if mqtt.topic_listeners.find(event['topic']) != nil 
        mqtt.topic_listeners[event['topic']](event['message'])
    end
end)