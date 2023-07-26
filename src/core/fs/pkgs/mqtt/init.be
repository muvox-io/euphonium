
var MQTT_STATE_DISABLED = "Disabled"
var MQTT_STATE_INVALID_CONFIG = "Invalid config"
var MQTT_STATE_CONNECTING = "Connecting"

class MQTTPlugin : Plugin
  var subscriptions
  var current_status
  def init()
      self.apply_default_values()
    
      self.name = "mqtt"
      self.theme_color = "#1DB954"
      self.display_name = "MQTT"
      self.type = "plugin"

      self.fetch_config()
      self.subscriptions = {}

      self.current_status = MQTT_STATE_DISABLED
    
      events.register_native("mqtt_publish_received", def (data)
        if self.subscriptions.find(data["topic"]) != nil
          var mqtt_callback = self.subscriptions[data["topic"]]
          # call subscription with received data
          mqtt_callback(data["message"])
        end
      end)
  end

  def make_form(ctx, state)
      var group = ctx.create_group('mqtt', { 'label': 'General' })

      group.checkbox_field('enable', {
          'label': "Connect to MQTT broker",
          'default': false,
          'group': 'mqtt'
      })

      if state.find('enable') != nil && state['enable']
        group.text_field('brokerUrl', {
          'label': "Broker host url",
          'default': "",
          'group': 'mqtt'
        })

        group.number_field('brokerPort', {
          'label': "Broker port",
          'default': 1883,
          'group': 'mqtt'
        })

        group.checkbox_field('authorize', {
          'label': "Authorize with broker",
          'default': false,
          'group': 'mqtt'
        })

        if state.find('authorize') != nil && state['authorize']
          group.text_field('username', {
            'label': "Username",
            'default': "",
            'group': 'mqtt'
          })

          group.text_field('password', {
            'label': "Password",
            'default': "",
            'group': 'mqtt'
          })
        end
      end
      self.add_apply_button(ctx, state)
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
    # TODO - set MQTT handlers here
  end

  def on_event(event, data)
    if event == EVENT_CONFIG_UPDATED || event == EVENT_PLUGIN_INIT
      if self.state.find("enable") != nil && self.state.find("enable")
        
        if self.state.find("brokerUrl") == nil
          self.current_status = MQTT_STATE_INVALID_CONFIG
          return
        end

        mqtt_username = ""
        mqtt_password = ""

        if self.state.find("authorize") != nil && self.state["authorize"]
          mqtt_username = self.state["username"]
          mqtt_password = self.state["password"]
        end

        self._connect(self.state["brokerUrl"], int(self.state["brokerPort"]), mqtt_username, mqtt_password)
        self.setup_basic_handlers()
      end
      if self.state.find("enable") != nil && !self.state.find("enable")
        self._disconnect()
      end
    end
  end
end

var mqtt = MQTTPlugin()
euphonium.register_plugin(mqtt)
