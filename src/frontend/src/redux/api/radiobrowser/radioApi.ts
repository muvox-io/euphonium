import eventSource from "../../../api/euphonium/eventSource";
import { PlaybackState } from "../../../api/euphonium/playback/models";
import { Station } from "../../../api/radiobrowser/models";
import { StationInReducer, favoritesReceivedFromApi, stationResultsReceivedFromApi } from "../../reducers/radiobrowserReducer";
import { EuphoniumApi } from "../euphonium/euphoniumApi";

let radioBrowserUrl = "https://radio-browser.gkindustries.pl";

export interface GetStationsByNameArgs {
  name: string;
  offset: number;
  limit: number;
}

const radioAPI = EuphoniumApi.injectEndpoints({
  endpoints: (builder) => ({
    getStationsByName: builder.mutation<Station[], GetStationsByNameArgs>({
      query: ({ name, limit, offset }) => ({
        url: `${radioBrowserUrl}/json/stations/byname/${name}?offset=${offset}&limit=${limit}&hidebroken=true`,
        method: "GET"
      }),
      onQueryStarted: async (args, { dispatch, queryFulfilled }) => {
        try {
          const { data } = await queryFulfilled;
          dispatch(stationResultsReceivedFromApi(data))
        } catch (e) { }
      },
    }),

    markStationFavorite: builder.mutation<StationInReducer[], StationInReducer>({
      query: (station) => ({
        url: '/radio/favorite',
        method: "POST",
        body: station
      }),
      onQueryStarted: async (args, { dispatch, queryFulfilled }) => {
        try {
          const { data } = await queryFulfilled;
          dispatch(favoritesReceivedFromApi(data))
        } catch (e) { }
      },
    }),

    getFavoriteStations: builder.query<StationInReducer[], void>({
      query: (station) => ({
        url: '/radio/favorite',
        method: "GET",
      }),
      onQueryStarted: async (args, { dispatch, queryFulfilled }) => {
        try {
          const { data } = await queryFulfilled;
          dispatch(favoritesReceivedFromApi(data))
        } catch (e) { }
      },
    }),

    playRadio: builder.mutation<void, { url: string, title: string, iconUrl: string }>({
      query: (body) => ({
        url: "/radio/play",
        method: "POST",
        body,
      })
    }),
  }),
});

export const { usePlayRadioMutation, useMarkStationFavoriteMutation, endpoints: radioApiEndpoints } = radioAPI;
