export interface WiFiNetwork {
    ssid: string;
    open: boolean;
}


export interface WiFiState {
    ssids: WiFiNetwork[];
    state: string;
    ipAddress: string;
}
