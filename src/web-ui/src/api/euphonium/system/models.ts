
export interface Connectivity {
    ipAddr?: string;
    ssid?: string;
    state?: 'CONNECTED_NO_INTERNET' | 'CONNECTED' | 'CONNECTING' | 'DISCONNECTED';
    type?: 'WIFI_STA' | 'WIFI_AP' | 'DEFAULT';
}


export interface EuphoniumInfo {
    version: string;
    connectivity: Connectivity;
    onboarding: boolean;
}

export interface AudioHardwareInfo {
    driver: string;
    driverType: "dac" | "amplifier";
    hardwareVolume: number;
    tuned: boolean;
}