class CSpotPlugin : Plugin
  def init()
      self.apply_default_values()

      self.name = "cspot"
      self.theme_color = "#1DB954"
      self.display_name = "Spotify (cspot)"
      self.type = "plugin"

      self.fetch_config()
  end

  def member(name)
    return get_native('cspot', name)
  end

  def make_form(ctx, state)
      var group_ctx = ctx.create_group('spotify', { 'label': 'General' })

      group_ctx.select_field('audioBitrate', {
          'label': "Audio bitrate",
          'default': "160",
          'values': ['320', '160', '96'],
          'group': 'spotify'
      })
      

      group_ctx.text_field('authData', {
          'label': "Saved auth data",
          'default': "",
          'group': 'spotify'
      })

      group_ctx.checkbox_field('overrideAp', {
          'label': "Override access point address",
          'default': "false",
          'group': 'spotify'
      })

      if state.find('overrideAp') != nil && state['overrideAp']
        group_ctx.text_field('apAddress', {
              'label': "Access point address",
              'default': "",
              'group': 'spotify'
          })
      end

      self.add_apply_button(ctx, state)
  end

  def register_discovery()
    # Prepare config blob with our name
    self._run_plugin(general_settings.get_name())

    # Create an endpoint that starts playback
    http.handle(HTTP_GET, "/spotify", def (req)
        req.write_raw(self._get_spotify_info(), 200, "application/json")
    end)

    # Create an endpoint that authenticates with spotify
    http.handle(HTTP_POST, "/spotify", def (req)
        # Pass authentication data to the native side, and write it's response
        var auth_data = self._authenticate_zeroconf(req.formencoded_body())
        self.state['authData'] = auth_data
        self.persist_config()

        # Write response to spotify
        req.write_json({
            "status": 101,
            "spotifyError": 0,
            "statusString": "ERROR-OK"
        })
    end)

    # Register MDNS service
    http.register_mdns(general_settings.get_name(), "_spotify-connect", "_tcp", { "CPath": "/spotify", "VERSION": "1.0", "Stack": "SP" })
  end

  def on_event(event, data)
      if event == EVENT_PLUGIN_INIT
        self.register_discovery()
        if (self.state.find('authData') != nil && self.state['authData'] != "")
            # self._authenticate_json(self.state['authData'])
        end
      end

      if event == EVENT_SET_PAUSE
          # cspot_set_pause(data)
      end

      if event == EVENT_VOLUME_UPDATED
          #cspot_set_volume_remote(data)
      end
  end
end

euphonium.register_plugin(CSpotPlugin())
