class MQTTPlugin : Plugin
  var subscriptions
  def init()
      self.apply_default_values()

      self.name = "mqtt"
      self.theme_color = "#1DB954"
      self.display_name = "MQTT"
      self.type = "plugin"

      self.fetch_config()
      self.subscriptions = {}

      events.register_native("mqtt_publish_received", def (data)
        print(data)
        if self.subscriptions.find(data["topic"]) != nil
          # call subscription with received data
          self.subscriptions[data["topic"]](data["message"])
        end
      end)
  end

  def make_form(ctx, state)
      ctx.create_group('mqtt', { 'label': 'General' })

      ctx.checkbox_field('enable', {
          'label': "Connect to MQTT broker",
          'default': "false",
          'group': 'mqtt'
      })

      if state.find('enable') != nil && state['enable'] == "true"
        ctx.text_field('brokerUrl', {
          'label': "Broker host url",
          'default': "",
          'group': 'mqtt'
        })

        ctx.number_field('brokerPort', {
          'label': "Broker port",
          'default': "1883",
          'group': 'mqtt'
        })
        
        ctx.text_field('username', {
          'label': "Username",
          'default': "",
          'group': 'mqtt'
        })

        ctx.text_field('password', {
          'label': "Password",
          'default': "",
          'group': 'mqtt'
        })
      end
  end

  def member(name)
    return get_native('mqtt', name)
  end

  def publish(topic, payload)
    self._publish(topic, payload)
  end

  def subscribe(topic, callback)
    # add to handler's map
    self.subscriptions[topic] = callback

    self._subscribe(topic)
  end

  def setup_basic_handlers()
    self.subscribe("euphonium/volume/set", def (payload)
      print("Setting volume to " + payload)
      euphonium.apply_volume(int(payload))
    end)

    self.subscribe("euphonium/volume/volume_up", def (payload)
      euphonium.apply_volume(playback_state.get_state()['settings']['volume'] + int(payload))
    end)

    self.subscribe("euphonium/volume/volume_down", def (payload)
      euphonium.apply_volume(playback_state.get_state()['settings']['volume'] - int(payload))
    end)

    self.subscribe("euphonium/playback/toggle", def (payload)
      if playback_state.get_state()['settings']['state'] == STATE_PLAYING
        
        playback_state.notify_state(STATE_PAUSED)
      else
        playback_state.notify_state(STATE_PLAYING)
      end
    end)
  end

  def on_event(event, data)
    if event == EVENT_CONFIG_UPDATED
      if self.state.find("enable") != nil && self.state.find("enable") == "true"
        self._connect(self.state["brokerUrl"], int(self.state["brokerPort"]), self.state["username"], self.state["password"])
        self.setup_basic_handlers()
      end
    end
  end
end

var mqtt = MQTTPlugin()
euphonium.register_plugin(mqtt)
