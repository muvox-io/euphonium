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
      ctx.create_group('snapcast', { 'label': 'General' })

      ctx.text_field('serverUrl', {
          'label': "Snapcast server URL",
          'default': "",
          'group': 'snapcast'
      })

      ctx.checkbox_field('enable', {
          'label': "Enable snapcast client",
          'default': "false",
          'group': 'snapcast'
      })
  end

  def on_event(event, data)
      if event == EVENT_SET_PAUSE
          # cspot_set_pause(data)
      end

      if event == EVENT_VOLUME_UPDATED
          #cspot_set_volume_remote(data)
      end
  end
end

euphonium.register_plugin(SnapcastPlugin())
