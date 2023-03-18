import APIAccessor from "../../APIAccessor";

export default class OtaAPI {
  
  constructor(private apiAccessor: APIAccessor) {}
  setOTAManifest = (manifest: any) =>
    this.apiAccessor.fetch<void>("POST", "/ota", manifest);
  getOTAManifest = () => this.apiAccessor.fetch<any>("GET", "/ota.config.json");
  triggerOTA = () => this.apiAccessor.fetch<void>("GET", "/start_ota");
}
