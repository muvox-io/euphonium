export interface EqSettings {
    low: number;
    mid: number;
    high: number;
}

export enum PlaybackStatus {
    Playing = "playing",
    Paused = "paused"
}

export interface PlaybackState {
    volume: number;
    song: SongPlaybackState;
    eq: EqSettings;
    status: PlaybackStatus;
}

export interface SongPlaybackState {
    artistName: string;
    sourceThemeColor: string;
    songName: string;
    sourceName: string;
    albumName: string;
    icon: string;
}
