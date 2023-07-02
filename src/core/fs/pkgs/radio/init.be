
class RadioPlugin : Plugin
  def init()
      self.apply_default_values()

      self.name = "radio"
      self.theme_color = "#1DB954"
      self.display_name = "Web Radio"
      self.type = "plugin"
      self.has_web_app = true

      self.fetch_config()

      # Create an endpoint that starts playback
      http.handle(HTTP_POST, "/radio/play", def (req)
        var jsonBody = req.json_body()
        if (jsonBody.find("url") != nil)
          self.handle_request_packet(jsonBody)
          req.write_json({ "success": true })
        else
          req.write_json({ "success": false })
        end
      end)

      # Save a radio station as favorite
      http.handle(HTTP_POST, "/radio/favorite", def (req)
        var radioJSONBody = req.json_body()
        var radios = self.get_favorite_stations()

        # Add received radio
        radios.push(radioJSONBody)

        # save results
        self.state['favorites'] = json.dump(radios)
        req.write_json(radios)
        self.persist_config()
      end)
  end

  def member(name)
    return get_native('radio', name)
  end

  def get_favorite_stations()
    var radios = []

    # Favorite radios get serialized as a JSON array in config
    if self.state.find('favorites') != nil
      radios = json.load(self.state['favorites'])
    end

    return radios
  end

  # prepares track metadata
  def handle_request_packet(request)
    if (request.find('title') == nil)
      request['title'] = 'HTTP Stream'
    end

    request['trackURI'] = 'http:' + request["url"]
    request['source'] = 'radio'

    playback_state.notify_playback(request)
    playback_state.notify_state(STATE_PLAYING)

    playback_state.query_context_uri("radio", request["url"])
  end

  def make_form(ctx, state)
      var group = ctx.create_group('radio', { 'label': 'General' })

      group.text_field('serverUrl', {
          'label': "Radio Browser Instance",
          'default': "https://radio-browser.gkindustries.pl/",
          'group': 'radio'
      })

      self.add_apply_button(ctx, state)
  end

  def on_event(event, data)
      if event == EVENT_SET_PAUSE
      end

      if event == EVENT_VOLUME_UPDATED
      end

      if event == EVENT_PLUGIN_INIT
        self._run_plugin()
      end
  end
end

euphonium.register_plugin(RadioPlugin())
