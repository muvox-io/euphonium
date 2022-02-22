enum PluginEntryType {
    Plugin = "plugin",
    System = "system",
    App = "app"
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

interface OTAManifest {
    url: string;
    sha256: string;
    name: string;
}

interface EuphoniumInfo {
    version: string;
    networkState: 'online' | 'offline' | 'recovery';
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

export type { PluginEntry, OTAManifest, DACPreset, EuphoniumInfo,WiFiNetwork, WiFiState }
export { PluginEntryType }
