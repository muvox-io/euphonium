import {
  EuphoniumInfo,
  WiFiState,
  DACPreset,
} from "./models";

export let apiUrl = "";
let currentVersion = "";

if (import.meta.env.MODE !== "production") {
  apiUrl = "http://localhost";
}

let eventsUrl = apiUrl + "/events";

const getDACPresets = async (): Promise<DACPreset[]> => {
  return await fetch(
    `https://raw.githubusercontent.com/feelfreelinux/euphonium/master/dacs.json`
  ).then((e) => e.json());
};

const playRadio = async (
  stationName: string,
  favicon: string,
  stationUrl: string,
  codec: string
): Promise<any> => {
  return await fetch(apiUrl + "/webradio", {
    method: "POST",
    headers: {
      Accept: "application/json",
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ stationUrl, codec, stationName, favicon }),
  }).then((e) => e.json());
};

const setPaused = async (isPaused: boolean) => {
  return await fetch(apiUrl + "/playback/status", {
    method: "POST",
    headers: {
      Accept: "application/json",
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ status: isPaused ? "paused" : "playing" }),
  });
};

const scanWifi = async () => {
  return await fetch(apiUrl + "/wifi/wifi_scan", { method: "GET" });
};

const connectToWifi = async (ssid: string, password: string) => {
  return await fetch(apiUrl + "/wifi/connect", {
    method: "POST",
    body: JSON.stringify({
      ssid,
      password,
    }),
  });
};

const triggerOTA = async () => {
  return await fetch(apiUrl + "/start_ota");
};

const getOTAManifest = async () => {
  return await fetch(apiUrl + "/web/ota.config.json", {
    method: "GET",
  }).then((e) => e.json());
};

const setOTAManifest = async (manifest: any) => {
  return await fetch(apiUrl + "/ota", {
    method: "POST",
    body: JSON.stringify(manifest)
  }).then((e) => e.json());
};

const getWifiStatus = async (): Promise<WiFiState> => {
  return await fetch(apiUrl + "/wifi/status", { method: "GET" }).then((e) =>
    e.json()
  );
};

const getInfo = async (): Promise<EuphoniumInfo> => {
  return await fetch(apiUrl + "/system", { method: "GET" }).then((e) => e.json());
};

let eventSource = new EventSource(eventsUrl);

export {
  eventSource,
  playRadio,
  setPaused,
  getWifiStatus,
  getInfo,
  getDACPresets,
  scanWifi,
  setOTAManifest,
  connectToWifi,
  getOTAManifest,
  triggerOTA
};
