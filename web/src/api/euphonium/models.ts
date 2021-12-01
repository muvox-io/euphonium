enum PluginEntryType {
    Plugin = "plugin",
    System = "system",
    App = "app"
}

enum PlaybackStatus {
    Playing = "playing",
    Paused = "paused"
}

interface PluginEntry {
    type: PluginEntryType;
    name: string;
    displayName: string;
}

enum ConfigurationFieldType {
    String = "string",
    StringList = "stringList"
}
interface ConfigurationField {
    type: ConfigurationFieldType,
    defaultValue: string;
    value: string;
    listValues: string[];
    tooltip: string;
    key: string;
}

interface EqSettings {
    low: number;
    mid: number;
    high: number;
}

interface PluginConfiguration {
    fields: ConfigurationField[];
    displayName: string;
}

interface PlaybackState {
    volume: number;
    song: SongPlaybackState;
    eq: EqSettings;
    status: PlaybackStatus;
}

interface SongPlaybackState {
    artistName: string;
    songName: string;
    sourceName: string;
    albumName: string;
    icon: string;
}


export type { PluginEntry, PluginConfiguration, EqSettings, ConfigurationField, PlaybackState }
export { ConfigurationFieldType, PluginEntryType, PlaybackStatus }
