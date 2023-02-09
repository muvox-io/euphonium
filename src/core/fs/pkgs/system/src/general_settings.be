class GeneralSettingsPlugin : Plugin
  def init()
      self.apply_default_values()

      self.name = "general_settings"
      self.theme_color = "#1DB954"
      self.display_name = "General settings"
      self.type = "system"

      self.fetch_config()

      # Apply display name to the native side
      core.set_display_name(self.get_name())
      euphonium.apply_volume(self.get_volume())
  end

  def make_form(ctx, state)
      ctx.create_group('general', { 'label': 'General' })

      ctx.text_field('playerName', {
          'label': "Player's name",
          'default': util.generate_device_name(),
          'group': 'general'
      })

      ctx.number_field('volume', {
          'label': "Saved volume",
          'default': 20,
          'group': 'general',
          'hidden': true
      })
  end

  def on_event(event, data)
  end

  def get_name()
    if self.state.find('playerName') == nil
      return util.generate_device_name()
    end
    return self.state['playerName']
  end

  def get_volume()
    if self.state.find('volume') == nil
      # default at hand volume
      return 50
    end
    return self.state['volume']
  end

  def set_volume(volume)
    self.state['volume'] = volume
    self.persist_config()
  end
end

var general_settings = GeneralSettingsPlugin()

euphonium.register_plugin(general_settings)
