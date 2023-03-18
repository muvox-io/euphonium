import APIAccessor from "../../APIAccessor";
import { ConfigurationField, PluginConfiguration, PluginEntry } from "./models";

export default class PluginsAPI {
  constructor(private apiAccessor: APIAccessor) {}

  /**
   * Gets the list of plugins
   * @returns
   */
  getPlugins = () => this.apiAccessor.fetch<PluginEntry[]>("GET", "/plugins");

  getPluginConfiguration = (pluginName: string) =>
    this.apiAccessor.fetch<PluginConfiguration>(
      "GET",
      `/plugins/${pluginName}`
    );

  updatePluginConfiguration = (
    pluginName: string,
    config: any,
    isPreview: boolean
  ) =>
    this.apiAccessor.fetch<PluginConfiguration>(
      "POST",
      `/plugins/${pluginName}`,
      { isPreview, configuration: config }
    );

  fieldsToValues = (fields: ConfigurationField[]) => {
    const values: { [key: string]: string } = {};
    fields.forEach((field) => {
      values[field.key] = field.value;
    });
    return values;
  };
}
