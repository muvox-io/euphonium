export interface EqSettings {
  low: number;
  mid: number;
  high: number;
}

export enum PlaybackStatus {
  Playing = "playing",
  Paused = "paused",
  Loading = "loading",
  EmptyQueue = "queue_empty"
}

export interface PlaybackSettings {
  state: PlaybackStatus;
  volume: number;
}

export interface PlaybackTrack {
  album: string;
  artist: string;
  iconUrl: string;
  source: string;
  title: string;
  trackURI: string;
  timestamp: number;
}

export interface PlaybackState {
  settings: PlaybackSettings;
  track: PlaybackTrack | null;
}

