import APIAccessor from "../../APIAccessor";
import { EqSettings, PlaybackState } from "./models";

export default class PlaybackAPI {
  constructor(private apiAccessor: APIAccessor) { }

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
      state: isPaused ? "paused" : "playing",
    });
  playRadio = (
    stationName: string,
    favicon: string,
    stationUrl: string
  ) =>
    this.apiAccessor.fetch<void>("POST", "/radio/play", {
      url: stationUrl,
      name: stationName,
      iconUrl: favicon
    });
}
