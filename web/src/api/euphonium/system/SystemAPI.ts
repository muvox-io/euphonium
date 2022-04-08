import APIAccessor from "../../APIAccessor";
import { EuphoniumInfo } from "./models";

export default class SystemAPI {
  constructor(private apiAccessor: APIAccessor) {}

  getSystemInfo = () => this.apiAccessor.fetch<EuphoniumInfo>("GET", "/system");
}
