enum PluginEntryType {
    Plugin = "plugin",
    System = "system"
}

interface PluginEntry {
    type: PluginEntryType;
    name: string;
    displayName: string;
}

enum ConfigurationFieldType {
    String = "string",
    StringList = "stringList"
}
interface ConfigurationField {
    type: ConfigurationFieldType,
    defaultValue: string;
    value: string;
    listValues: string[];
    tooltip: string;
    key: string;
}


export type { PluginEntry, ConfigurationField }
export { ConfigurationFieldType, PluginEntryType }