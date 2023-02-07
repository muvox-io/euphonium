class GeneralSettingsPlugin : Plugin
  def init()
      self.apply_default_values()

      self.name = "general_settings"
      self.theme_color = "#1DB954"
      self.display_name = "General settings"
      self.type = "system"

      self.fetch_config()
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
    if event == EVENT_CONFIG_UPDATED
    end
  end

  def get_name()
    return self.state['playerName']
  end
end

var general_settings = GeneralSettingsPlugin()

euphonium.register_plugin(general_settings)
