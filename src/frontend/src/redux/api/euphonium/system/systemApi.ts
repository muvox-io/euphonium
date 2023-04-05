import { createApi, fetchBaseQuery } from "@reduxjs/toolkit/query/react";
import getBaseUrl from "../../../../api/euphonium/baseUrl";
import { EuphoniumInfo } from "../../../../api/euphonium/system/models";

export const SystemApi = createApi({
  reducerPath: "systemApi",
  baseQuery: fetchBaseQuery({ baseUrl: getBaseUrl() + "/system" }),
  endpoints: (builder) => ({
    getSystemInfo: builder.query<EuphoniumInfo, void>({
      query: () => "info",
    }),
  }),
});

export const { useGetSystemInfoQuery } = SystemApi;
