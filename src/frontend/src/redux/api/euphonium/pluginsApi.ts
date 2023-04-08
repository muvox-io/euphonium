import { PluginConfiguration } from "../../../api/euphonium/plugins/models";
import { stateFromServerApplied } from "../../reducers/pluginConfigurationsReducer";
import { EuphoniumApi } from "./euphoniumApi";
import { PostPluginConfigurationArgs } from "./pluginsApiModel";

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
      onQueryStarted: async (pluginName, { dispatch, queryFulfilled }) => {
        try {
          const { data } = await queryFulfilled;
          dispatch(stateFromServerApplied({ pluginName, state: data.state }));
        } catch (e) {}
      },
    }),

    postPluginConfiguration: builder.mutation<
      PluginConfiguration,
      PostPluginConfigurationArgs
    >({
      query: (args) => ({
        url: `/plugins/${args.pluginName}`,
        method: "POST",
        body: args,
      }),
      onQueryStarted: async (
        { pluginName },
        { dispatch, queryFulfilled }
      ) => {
        try {
          const { data } = await queryFulfilled;
          console.log("DATA FROM SERVER", data);
          dispatch(stateFromServerApplied({ pluginName, state: data.state }));
          dispatch(
            pluginsApi.util.updateQueryData(
              "getPluginConfiguration",
              pluginName,
              (draft) => data
            )
          );
        } catch (e) {}
      },
    }),
  }),
});

export const {
  useGetGlobalModalsQuery,
  useGetPluginConfigurationQuery,
  endpoints: pluginsApiEndpoints,
} = pluginsApi;
