export interface PluginInterfaceEvent {
  fieldId: string;
  name: string;
  payload?: any;
}

export interface PostPluginConfigurationArgs {
  pluginName: string;
  isPreview: boolean;
  state: any;
  events: PluginInterfaceEvent[];
}
