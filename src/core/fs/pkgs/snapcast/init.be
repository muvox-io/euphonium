class SnapcastPlugin : Plugin
  def init()
      self.apply_default_values()

      self.name = "snapcast"
      self.theme_color = "#1DB954"
      self.display_name = "Snapcast"
      self.type = "plugin"

      self.fetch_config()
  end

  def make_form(ctx, state)
      var group = ctx.create_group('snapcast', { 'label': 'General' })

      group.text_field('serverUrl', {
          'label': "Server address",
          'default': "",
          'group': 'snapcast'
      })

      group.number_field('serverPort', {
          'label': "Server port",
          'default': "1704",
          'group': 'snapcast'
      })

      group.checkbox_field('enable', {
          'label': "Enable snapcast client",
          'default': "false",
          'group': 'snapcast'
      })

      self.add_apply_button(ctx, state)
  end

  def member(name)
    return get_native('snapcast', name)
  end

  def on_event(event, data)
      if event == EVENT_CONFIG_UPDATED
        if self.state.find("enable") != nil && self.state.find("enable") == "true"
          self._connect(self.state["serverUrl"], int(self.state["serverPort"]))
        else
          self._disconnect()
        end
      end
  end
end

euphonium.register_plugin(SnapcastPlugin())
