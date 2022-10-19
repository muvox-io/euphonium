import APIAccessor from "../../APIAccessor";
import { WiFiState } from "./models";


export default class WiFiAPI {
  constructor(private apiAccessor: APIAccessor) {}
  getWifiStatus = () =>
    this.apiAccessor.fetch<WiFiState>("GET", "/wifi/status");
  wifiScan = () => this.apiAccessor.fetch<void>("GET", "/wifi/wifi_scan");
  connectToWifi = (ssid: string, password: string) => {
    return this.apiAccessor.fetch<void>(
      "POST",
      "/wifi/connect",
      { ssid, password }
    );
  }
}
