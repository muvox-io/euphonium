import {
  ConfigurationField,
  PlaybackState,
  PluginConfiguration,
  PluginEntry,
  EqSettings,
  EuphoniumInfo,
  WiFiState,
  DACPreset,
} from "./models";

let apiUrl = "";

if (import.meta.env.MODE !== "production") {
  apiUrl = "http://192.168.254.123:80";
}

let eventsUrl = apiUrl + "/events";

const getPlugins = async (): Promise<PluginEntry[]> => {
  return await fetch(apiUrl + "/plugins")
    .then((e) => e.json())
    .then((e) => e.map((e: any) => e as PluginEntry));
};

const getPlaybackState = async (): Promise<PlaybackState> => {
  return await fetch(apiUrl + "/playback")
    .then((e) => e.json())
    .then((e) => e as PlaybackState);
};

const getPluginConfiguration = async (
  pluginName: string
): Promise<PluginConfiguration> => {
  return await fetch(`${apiUrl}/plugins/${pluginName}`)
    .then((e) => e.json())
    .then((e) => {
      return {
        displayName: e.displayName,
        fields: Object.keys(e["configSchema"])
          .map((key) => {
            return {
              ...e["configSchema"][key],
              key,
            } as ConfigurationField;
          })
          .reverse(),
      };
    });
};

const getDACPresets = async (): Promise<DACPreset[]> => {
  return await fetch(
    `https://gist.githubusercontent.com/feelfreelinux/e9b0866342350536fb46f9155ff6ec19/raw/dacs.json`
  ).then((e) => e.json());
};

const updatePluginConfValues = async (
  pluginName: string,
  config: any
): Promise<PluginConfiguration> => {
  return await fetch(`${apiUrl}/plugins/${pluginName}`, {
    method: "POST",
    headers: {
      Accept: "application/json",
      "Content-Type": "application/json",
    },
    body: JSON.stringify(config),
  })
    .then((e) => e.json())
    .then((e) => {
      return {
        displayName: e.displayName,
        fields: Object.keys(e["configSchema"]).map((key) => {
          return {
            ...e["configSchema"][key],
            key,
          } as ConfigurationField;
        }),
      };
    });
};

const updatePluginConfiguration = async (
  pluginName: string,
  config: ConfigurationField[]
): Promise<PluginConfiguration> => {
  let mappedConfig = {} as any;

  config.forEach((e) => {
    mappedConfig[e.key] = e.value;
  });

  return updatePluginConfValues(pluginName, mappedConfig);
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

const updateEq = async (settings: EqSettings): Promise<any> => {
  return await fetch(apiUrl + "/eq", {
    method: "POST",
    headers: {
      Accept: "application/json",
      "Content-Type": "application/json",
    },
    body: JSON.stringify(settings),
  }).then((e) => e.json());
};

const updateVolume = async (volume: number): Promise<any> => {
  return await fetch(apiUrl + "/volume", {
    method: "POST",
    headers: {
      Accept: "application/json",
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ volume }),
  }).then((e) => e.json());
};

const setPaused = async (isPaused: boolean) => {
  return await fetch(apiUrl + "/play", {
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
const getWifiStatus = async (): Promise<WiFiState> => {
  return await fetch(apiUrl + "/wifi/status", { method: "GET" }).then((e) =>
    e.json()
  );
};

const getInfo = async (): Promise<EuphoniumInfo> => {
  return await fetch(apiUrl + "/info", { method: "GET" }).then((e) => e.json());
};

let eventSource = new EventSource(eventsUrl);

export {
  getPlugins,
  eventSource,
  updatePluginConfiguration,
  playRadio,
  getPluginConfiguration,
  getPlaybackState,
  updateEq,
  updateVolume,
  updatePluginConfValues,
  setPaused,
  getWifiStatus,
  getInfo,
  getDACPresets,
  scanWifi,
  connectToWifi,
};
