import APIAccessor from "../../APIAccessor";
import { EqSettings, PlaybackState } from "./models";

export default class PlaybackAPI {
  constructor(private apiAccessor: APIAccessor) {}

  updateEq = (settings: EqSettings, persist = false) =>
    this.apiAccessor.fetch<void>("POST", "/playback/eq", {
      ...settings,
      persist,
    });
  getPlaybackState = () =>
    this.apiAccessor.fetch<PlaybackState>("GET", "/playback");
  updateVolume = (volume: number, persist = false) =>
    this.apiAccessor.fetch<void>("POST", "/playback/volume", {
      volume,
      persist,
    });
  setPaused = (isPaused: boolean) =>
    this.apiAccessor.fetch<void>("POST", "/playback/status", {
      status: isPaused ? "paused" : "playing",
    });
  playRadio = (
    stationName: string,
    favicon: string,
    stationUrl: string,
    codec: string
  ) =>
    this.apiAccessor.fetch<void>("POST", "/webradio", {
      stationUrl,
      codec,
      stationName,
      favicon,
    });
}
