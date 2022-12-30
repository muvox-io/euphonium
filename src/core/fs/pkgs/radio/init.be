
class RadioPlugin : Plugin
  def init()
      self.apply_default_values()

      self.name = "radio"
      self.theme_color = "#1DB954"
      self.display_name = "Web Radio"
      self.type = "plugin"

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
  end

  def member(name)
    return get_native('radio', name)
  end

  # prepares track metadata
  def handle_request_packet(request)
    self._query_url(request)

    if (request.find('name') == nil)
      request['name'] = 'HTTP Stream'
    end

    request['trackURI'] = 'http:' + request["url"]
    request['source'] = 'radio'

    playback_state.notify_playback(request)
    playback_state.notify_state(STATE_PLAYING)
  end

  def make_form(ctx, state)
      ctx.create_group('radio', { 'label': 'General' })

      ctx.text_field('receiverName', {
          'label': "Speaker's name",
          'default': "Euphonium ",
          'group': 'radio'
      })
  end

  def on_event(event, data)
      if event == EVENT_SET_PAUSE
          # cspot_set_pause(data)
      end

      if event == EVENT_VOLUME_UPDATED
          #cspot_set_volume_remote(data)
      end

      if event == EVENT_PLUGIN_INIT
        self._run_plugin()
      end
  end
end

euphonium.register_plugin(RadioPlugin())
