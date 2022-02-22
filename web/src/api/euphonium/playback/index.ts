import { apiUrl } from "../api";
import { EqSettings, PlaybackState } from "./models";

export const updateEq = async (settings: EqSettings, persist = false): Promise<any> => {
    return await fetch(apiUrl + "/playback/eq", {
        method: "POST",
        headers: {
            Accept: "application/json",
            "Content-Type": "application/json",
        },
        body: JSON.stringify({ ...settings, persist } ),
    }).then((e) => e.json());
};

export const getPlaybackState = async (): Promise<PlaybackState> => {
    return await fetch(apiUrl + "/playback")
        .then((e) => e.json())
        .then((e) => e as PlaybackState);
};

export const updateVolume = async (volume: number, persist = false): Promise<any> => {
    return await fetch(apiUrl + "/playback/volume", {
        method: "POST",
        headers: {
            Accept: "application/json",
            "Content-Type": "application/json",
        },
        body: JSON.stringify({ volume, persist }),
    }).then((e) => e.json());
};

export * from './models'