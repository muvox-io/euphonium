import {
  ConfigurationField,
  PlaybackState,
  PluginConfiguration,
  PluginEntry,
  EqSettings
} from "./models";

let apiUrl = "";


if (import.meta.env.MODE !== 'production') {
    apiUrl = "http://localhost:80";
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

  return await fetch(`${apiUrl}/plugins/${pluginName}`, {
    method: "POST",
    headers: {
      Accept: "application/json",
      "Content-Type": "application/json",
    },
    body: JSON.stringify(mappedConfig),
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

const updateEq = async (
  settings: EqSettings): Promise<any> => {
  return await fetch(apiUrl + "/eq", {
    method: "POST",
    headers: {
      Accept: "application/json",
      "Content-Type": "application/json",
    },
    body: JSON.stringify(settings),
  }).then((e) => e.json());
};

const updateVolume = async (
  volume: number): Promise<any> => {
  return await fetch(apiUrl + "/volume", {
    method: "POST",
    headers: {
      Accept: "application/json",
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ volume }),
  }).then((e) => e.json());
};

let eventSource = new EventSource("/events");

export {
  getPlugins,
  eventSource,
  updatePluginConfiguration,
  playRadio,
  getPluginConfiguration,
  getPlaybackState,
  updateEq,
  updateVolume
};
