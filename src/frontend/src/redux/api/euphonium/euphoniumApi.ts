import { createApi, fetchBaseQuery } from "@reduxjs/toolkit/query/react";
import getBaseUrl from "../../../api/euphonium/baseUrl";
import { PlaybackState } from "../../../api/euphonium/playback/models";
import { EuphoniumInfo } from "../../../api/euphonium/system/models";

export const EuphoniumApi = createApi({
  reducerPath: "euphoniumApi",
  baseQuery: fetchBaseQuery({ baseUrl: getBaseUrl() }),
  endpoints: (builder) => ({
    getSystemInfo: builder.query<EuphoniumInfo, void>({
      query: () => "/system/info",
    }),
  }),
  
});

export const { useGetSystemInfoQuery } = EuphoniumApi;
