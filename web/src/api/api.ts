import { ConfigurationField, PlaybackState, PluginEntry } from "./models";

const getPlugins = async (): Promise<PluginEntry[]> => {
    return await fetch("http://localhost:2137/plugins")
        .then((e) => e.json())
        .then((e) => e.map((e: any) => e as PluginEntry));
}

const getPlaybackState = async (): Promise<PlaybackState> => {
    return await fetch("http://localhost:2137/playback")
        .then((e) => e.json())
        .then((e) => e as PlaybackState);
}

const getPluginConfiguration = async (pluginName: string): Promise<ConfigurationField[]> => {
    return await fetch(`http://localhost:2137/plugins/${pluginName}`)
        .then((e) => e.json())
        .then((e) => {
            return Object.keys(e).map((key) => {
                return {
                    ...e[key],
                    key
                } as ConfigurationField
            });
        })
}

const updateConfiguration = async (pluginName: string): Promise<ConfigurationField[]> => {
    return await fetch(`http://localhost:2137/plugins/${pluginName}`)
        .then((e) => e.json())
        .then((e) => {
            return Object.keys(e).map((key) => {
                return {
                    ...e[key],
                    key
                } as ConfigurationField
            });
        })
}

export { getPlugins, getPluginConfiguration, getPlaybackState }