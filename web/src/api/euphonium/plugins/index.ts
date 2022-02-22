import { apiUrl } from "../api";
import { PluginEntry } from "../models";
import { PluginConfiguration, ConfigurationField } from "./models";

// gets the list of plugins
export const getPlugins = async (): Promise<PluginEntry[]> => {
    return await fetch(apiUrl + "/plugins")
        .then((e) => e.json())
};

// gets the plugin configuration
export const getPluginConfiguration = async (
    pluginName: string
): Promise<PluginConfiguration> => {
    return await fetch(`${apiUrl}/plugins/${pluginName}`)
        .then((e) => e.json())
};

// maps the plugin configuration schema to a form that can be used to configure the plugin
export const fieldsToValues = (fields: ConfigurationField[]) => {
    const values: { [key: string]: string } = {};
    fields.forEach((field) => {
        values[field.key] = field.value;
    });
    return values;
}

// updates the plugin configuration. isPreview is true if the update is a preview of the configuration
export const updatePluginConfiguration = async (
    pluginName: string,
    config: any,
    isPreview: boolean
): Promise<PluginConfiguration> => {
    return await fetch(`${apiUrl}/plugins/${pluginName}`, {
        method: "POST",
        headers: {
            Accept: "application/json",
            "Content-Type": "application/json",
        },
        body: JSON.stringify({ isPreview, configuration: config }),
    }).then((e) => e.json());
};


export * from "./models";