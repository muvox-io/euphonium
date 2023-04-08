export enum PluginEntryType {
  Plugin = "plugin",
  System = "system",
  App = "app",
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
  CHECKBOX = "checkbox_field",
  LINK_BUTTON = "link_button",
  MODAL_CONFIRM = "modal_confirm",
  BUTTON_FIELD = "button_field",
  MODAL_GROUP = "modal_group",
}

export interface ConfigurationField {
  type: ConfigurationFieldType;
  key: string;

  children?: ConfigurationField[];

  label?: string;
  default?: string;
  hidden?: boolean;
  values?: string[];
  hint?: string;
  placeholder?: string;
  group?: string;
  link?: string;
  okValue?: string;
  cancelValue?: string;
  buttonText?: string;

  // modal group
  priority?: number;
  dismissable?: boolean;
  title?: string;
}

export interface PluginConfiguration {
  configSchema: ConfigurationField[];
  themeColor: string;
  displayName: string;
  state: any;
}
