STATE_PAUSED = 'paused'
STATE_LOADING = 'loading'
STATE_PLAYING = 'playing'
STATE_EMPTY = 'queue_empty'

class PlaybackState
  var current_track
  var settings

  def init()
    self.current_track = self.create_default_track()
    self.settings = self.create_default_settings()
  end

  # initializes the current_track object
  def create_default_track()
    return {
      'title': '',
      'trackURI': '',
      'iconUrl': '',
      'artist': '',
      'album': '',
      'source': ''
    }
  end

  def notify_playback(track)
    self.current_track = track
    self.update_remote()
  end

  def notify_state(state)
    self.settings['state'] = state

    if state == STATE_PLAYING
      core.trigger_pause(false)
    end

    if state == STATE_PAUSED
      core.trigger_pause(true)
    end

    self.update_remote()
  end

  # Called in order to send current state to the websocket connection
  def update_remote()
    http.emit_event('playback', self.get_state())
  end

  def get_state()
    return { 'settings': self.settings, 'track': self.current_track }
  end

  # intializes the settings object
  def create_default_settings()
    return {
      'volume': 0,
      'state': STATE_EMPTY
    }
  end
end

var playback_state = PlaybackState()
