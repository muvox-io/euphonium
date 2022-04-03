export enum PluginEntryType {
    Plugin = "plugin",
    System = "system",
    App = "app"
}

export interface PluginEntry {
    type: PluginEntryType;
    name: string;
    displayName: string;
}

export enum ConfigurationFieldType {
    GROUP = "group",
    TEXT = "text_field",
    SELECT = "select_field",
    NUMBER = "number_field",
    CHECKBOX = "checkbox_field"
}

export interface ConfigurationField {
    type: ConfigurationFieldType;
    key: string;
    value: string;
    label?: string;
    default?: string;
    values?: string[];
    hint?: string;
    placeholder?: string;
    group?: string;
}

export interface PluginConfiguration {
    configSchema: ConfigurationField[];
    themeColor: string;
    displayName: string;
}
