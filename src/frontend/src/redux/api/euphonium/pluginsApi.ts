import eventSource from "../../../api/euphonium/eventSource";
import { PlaybackState } from "../../../api/euphonium/playback/models";
import { PluginConfiguration } from "../../../api/euphonium/plugins/models";
import { EuphoniumApi } from "./euphoniumApi";

const pluginsApi = EuphoniumApi.injectEndpoints({
  endpoints: (builder) => ({
    /**
     * Gets plugin names which want to show a global modal.
     */
    getGlobalModals: builder.query<string[], void>({
      query: () => "/global-modals",
    }),

    getPluginConfiguration: builder.query<PluginConfiguration, string>({
      query: (pluginName) => `/plugins/${pluginName}`,
    }),
  }),
});

export const { useGetGlobalModalsQuery, useGetPluginConfigurationQuery } = pluginsApi;
