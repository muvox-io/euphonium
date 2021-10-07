import { ConfigurationField, PluginEntry } from "./models";

const getPlugins = async (): Promise<PluginEntry[]> => {
    return await fetch("http://localhost:2137/plugins")
        .then((e) => e.json())
        .then((e) => e.map((e: any) => e as PluginEntry));
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

export { getPlugins, getPluginConfiguration }