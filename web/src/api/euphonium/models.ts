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

interface WiFiNetwork {
    ssid: string;
    open: boolean;
}

interface WiFiState {
    ssids: WiFiNetwork[];
    state: string;
    ipAddress: string;
}

interface EuphoniumInfo {
    version: string;
    networkState: 'online' | 'offline';
}

enum ConfigurationFieldType {
    String = "string",
    Number = "number",
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

interface DACPreset {
    board: string;
    dac: string;
    mclk: string;
    bck: string;
    ws: string;
    data: string;
    scl: string;
    sda: string;
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


export type { PluginEntry, PluginConfiguration, EqSettings, DACPreset, EuphoniumInfo, ConfigurationField, PlaybackState, WiFiNetwork, WiFiState }
export { ConfigurationFieldType, PluginEntryType, PlaybackStatus }
