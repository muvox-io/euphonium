class BluetoothPlugin : Plugin
  var meta_counter
  def init()
      self.apply_default_values()

      self.name = "bluetooth"
      self.theme_color = "#1DB954"
      self.display_name = "Bluetooth"
      self.type = "plugin"

      self.fetch_config()

      self.meta_counter = 0
      # Register to receive Bluetooth playback metadata
      events.register_native("bluetooth_metadata", def (data)
        self.handle_metadata(data)
      end)
  end

  def handle_metadata(data)
    if self.meta_counter < 4
      self.meta_counter += 1
      return
    end

    self.meta_counter = 0

    var track = {
      'title': 'Bluetooth playback',
      'trackURI': 'bluetooth:stream',
      'iconUrl': 'icons:bluetooth',
      'artist': '',
      'album': '',
      'source': 'bluetooth'
    }

    if data.find('artist') != nil
      track['artist'] = data['artist']
    end

    if data.find('title') != nil
      track['title'] = data['title']
    end

    if data.find('album') != nil
      track['album'] = data['album']
    end
    print(track)

    playback_state.notify_playback(track)
    playback_state.notify_state(STATE_PLAYING)
  end

  def member(name)
    return get_native('bluetooth', name)
  end

  def make_form(ctx, state)
      ctx.create_group('bluetooth', { 'label': 'General' })
  end

  def on_event(event, data)
      if event == EVENT_PLUGIN_INIT
        self._run_plugin(general_settings.get_name())
      end
  end
end

euphonium.register_plugin(BluetoothPlugin())
