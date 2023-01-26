import APIAccessor from "../../APIAccessor";
import { EqSettings, PlaybackState, PlaybackTrack } from "./models";

const debounce = function <T extends (...args: any[]) => void>(
  callback: T,
  debounceDelay: number = 300,
  immediate: boolean = false
) {
  let timeout: ReturnType<typeof setTimeout> | null;

  return function <U>(this: U, ...args: Parameters<typeof callback>) {
    const context = this;

    if (immediate && !timeout) {
      callback.apply(context, args)
    }
    if (typeof timeout === "number") {
      clearTimeout(timeout);
    }
    timeout = setTimeout(() => {
      timeout = null;
      if (!immediate) {
        callback.apply(context, args)
      }
    }, debounceDelay);
  }
}

function throttle<Args extends unknown[]>(fn: (...args: Args) => void, cooldown: number) {
  let lastArgs: Args | undefined;

  const run = () => {
    if (lastArgs) {
      fn(...lastArgs);
      lastArgs = undefined;
    }
  };

  const throttled = (...args: Args) => {
    const isOnCooldown = !!lastArgs;

    lastArgs = args;

    if (isOnCooldown) {
      return;
    }

    window.setTimeout(run, cooldown);
  };

  return throttled;
}

export default class PlaybackAPI {
  constructor(private apiAccessor: APIAccessor) { }

  updateEq = (settings: EqSettings, persist = false) =>
    this.apiAccessor.fetch<void>("POST", "/playback/eq", {
      ...settings,
      persist,
    });
  getPlaybackState = () =>
    this.apiAccessor.fetch<PlaybackState>("GET", "/playback");

  updateVolume = (volume: number, persist = false) => {
    this.apiAccessor.fetch<void>("POST", "/playback/volume", {
      volume,
      persist,
    });
  }

  updateVolumeThrottled = throttle(this.updateVolume, 200);

  setPaused = (isPaused: boolean) =>
    this.apiAccessor.fetch<void>("POST", "/playback/status", {
      state: isPaused ? "paused" : "playing",
    });

  getRecentlyPlayed = () => this.apiAccessor.fetch<PlaybackTrack[]>("GET", "/playback/recent");

  playRadio = (
    stationName: string,
    favicon: string,
    stationUrl: string
  ) =>
    this.apiAccessor.fetch<void>("POST", "/radio/play", {
      url: stationUrl,
      title: stationName,
      iconUrl: favicon
    });
}
