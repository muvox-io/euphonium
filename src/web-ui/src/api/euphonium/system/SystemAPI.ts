import APIAccessor from "../../APIAccessor";
import { Connectivity, EuphoniumInfo } from "./models";

export default class SystemAPI {
  constructor(private apiAccessor: APIAccessor) {}

  getSystemInfo = () => this.apiAccessor.fetch<EuphoniumInfo>("GET", "/system/info");

  getConnectivityInfo = () => this.apiAccessor.fetch<Connectivity>("GET", "/system/connectivity");
}
