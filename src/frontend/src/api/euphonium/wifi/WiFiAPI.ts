import APIAccessor from "../../APIAccessor";


export default class WiFiAPI {
  constructor(private apiAccessor: APIAccessor) {}
  wifiScan = () => this.apiAccessor.fetch<void>("GET", "/wifi/scan");
  connectToWifi = (ssid: string, password: string) => {
    return this.apiAccessor.fetch<void>(
      "POST",
      "/wifi/connect",
      { ssid, password }
    );
  }
}
