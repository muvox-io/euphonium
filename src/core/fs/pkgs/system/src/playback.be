STATE_PAUSED = 'paused'
STATE_LOADING = 'loading'
STATE_PLAYING = 'playing'
STATE_EMPTY = 'queue_empty'

class PlaybackState
  var current_track
  var settings

  def init
    self.current_track = self.create_default_track()
    self.settings = self.create_default_settings()
  end

  # initializes the current_track object
  def create_default_track()
    return {
      'title': '',
      'trackURI': '',
      'artist': '',
      'album': '',
      'source': ''
    }
  end

  # intializes the settings object
  def create_default_settings()
    return {
      'volume': 0,
      'state': STATE_EMPTY
    }
  end
end
