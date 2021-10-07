import { PluginEntry } from "./models";

const getPlugins = async (): Promise<PluginEntry[]> => {
    return await fetch("http://localhost:2137/plugins")
        .then((e) => e.json())
        .then((e) => e.map((e: any) => e as PluginEntry));
}

export { getPlugins }