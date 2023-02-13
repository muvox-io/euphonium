import APIAccessor from "../../APIAccessor";
import { AudioHardwareInfo, Connectivity, EuphoniumInfo } from "./models";

export default class SystemAPI {
  constructor(private apiAccessor: APIAccessor) { }

  getSystemInfo = () => this.apiAccessor.fetch<EuphoniumInfo>("GET", "/system/info");

  getConnectivityInfo = () => this.apiAccessor.fetch<Connectivity>("GET", "/system/connectivity");

  getAudioHardwareInfo = () => this.apiAccessor.fetch<AudioHardwareInfo>("GET", "/system/audio_hardware");

  setAudioHardwareMaxVolume = (maxVolume: number) => this.apiAccessor.fetch<AudioHardwareInfo>("POST", "/system/audio_hardware", { "max_volume": maxVolume });
}
