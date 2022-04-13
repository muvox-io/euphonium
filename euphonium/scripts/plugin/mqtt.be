class MQTTPlugin : Plugin
    def init()
        self.apply_default_values()
        self.name = "mqtt"
        self.theme_color = "#d2c464"
        self.display_name = "MQTT"
        self.type = "plugin"
    end

    def make_form(ctx, state)
        ctx.create_group('mqtt', { 'label': 'General' })
        ctx.checkbox_field('enableMqtt', {
            'label': "Connect with MQTT Broker",
            'default': "false",
            'group': 'mqtt'
        })

        if state.find('enableMqtt') != nil && state['enableMqtt'] == 'true'
            ctx.text_field('mqttBrokerUrl', {
                'label': "Broker address",
                'default': "",
                'group': 'mqtt'
            })

            ctx.number_field('mqttPort', {
                'label': "Broker port",
                'default': "",
                'group': 'mqtt'
            })

            ctx.text_field('mqttUsername', {
                'label': "Username",
                'default': "",
                'group': 'mqtt'
            })

            ctx.text_field('mqttPassword', {
                'label': "Password",
                'default': "",
                'group': 'mqtt'
            })

            ctx.checkbox_field('enableHomeAssistant', {
                'label': "Enable Home Assistant integration",
                'default': "false",
                'group': 'mqtt'
            })
        end
    end

    # Contains sample home assistant integration @TODO Extend this
    def setup_home_assistant()
            mqtt.publish("homeassistant/switch/euphonium_volumeup/config", json.dump({
                'name': 'Euphonium Volume UP',
                'command_topic': 'euphonium/volume_up/set',
            }))
            mqtt.publish("homeassistant/switch/euphonium_volumedown/config", json.dump({
                'name': 'Euphonium Volume DOWN',
                'command_topic': 'euphonium/volume_down/set',
            }))
            
            mqtt.subscribe("euphonium/volume_up/set")
            mqtt.on_publish("euphonium/volume_up/set", def (volume) 
                euphonium.apply_volume(euphonium.last_volume + 5)
            end)

            mqtt.subscribe("euphonium/volume_down/set")
            mqtt.on_publish("euphonium/volume_down/set", def (volume) 
                euphonium.apply_volume(euphonium.last_volume - 5)
            end)
    end

    # connects with an MQTT Broker
    def mqtt_connect(mqtt_config)
        if mqtt.connect(mqtt_config['mqttBrokerUrl'], mqtt_config['mqttPort'], mqtt_config['mqttUsername'], mqtt_config['mqttPassword'])
            euphonium.send_notification("success", "mqtt", "MQTT Connected to broker")
            if mqtt_config['enableHomeAssistant']
                self.setup_home_assistant()
            end
        else
            euphonium.send_notification("error", "mqtt", "MQTT cannot connect to broker")
        end
    end

    def on_event(event, data)
        if event == EVENT_VOLUME_UPDATED
            # mqtt.publish('euphonium/volume/state', str(data))
        end

        if event == EVENT_CONFIG_UPDATED
            self.mqtt_connect(data)
        end

        if event == EVENT_PLUGIN_INIT
            if self.state.find('enableMqtt') != nil && self.state['enableMqtt']
                self.mqtt_connect(self.state)
            end
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