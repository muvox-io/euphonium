export interface WiFiNetwork {
    ssid: string;
    open: boolean;
}

export interface WiFiEventBody {
    networks?: WiFiNetwork[];
    error?: boolean;
    scanning?: boolean;
}

export interface ConnectivityEvent {
    ip?: string;
    ssid?: string;
    state?: 'CONNECTED_NO_INTERNET' | 'CONNECTED' | 'CONNECTING' | 'DISCONNECTED';
    type?: 'WIFI_STA' | 'WIFI_AP' | 'DEFAULT';
    body?: WiFiEventBody;
}