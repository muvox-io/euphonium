import eventSource from "../../../api/euphonium/eventSource";
import { PlaybackState } from "../../../api/euphonium/playback/models";
import { playbackStateReceivedFromServer } from "../../reducers/localPlaybackState";
import { EuphoniumApi } from "./euphoniumApi";

const playbackApi = EuphoniumApi.injectEndpoints({
  endpoints: (builder) => ({
    getPlaybackState: builder.query<PlaybackState, void>({
      query: () => "/playback",
      onQueryStarted: async (arg, { dispatch, queryFulfilled }) => {
        try {
          const { data } = await queryFulfilled;

          dispatch(playbackStateReceivedFromServer(data));
        } catch (e) {
          console.log("ERROR", e);
        }
      },
      onCacheEntryAdded: async (
        arg,
        { cacheDataLoaded, cacheEntryRemoved, updateCachedData, dispatch }
      ) => {
        const listener = (data: PlaybackState) => {
          updateCachedData(() => {
            return data;
          });

          dispatch(playbackStateReceivedFromServer(data));
        };
        try {
          await cacheDataLoaded;
          eventSource.on("playback", listener);
        } catch (e) {}
        await cacheEntryRemoved;
        eventSource.removeListener("playback", listener);
      },
    }),
    updateVolume: builder.mutation<void, { volume: number; persist: boolean }>({
      query: (body) => ({
        url: "/playback/volume",
        method: "POST",
        body,
      }),
    }),
  }),
});

export const { useGetPlaybackStateQuery, endpoints: playbackApiEndpoints } =
  playbackApi;
