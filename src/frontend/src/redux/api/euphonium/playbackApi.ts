import eventSource from "../../../api/euphonium/eventSource";
import { PlaybackState } from "../../../api/euphonium/playback/models";
import { EuphoniumApi } from "./euphoniumApi";

const playbackApi = EuphoniumApi.injectEndpoints({
  endpoints: (builder) => ({
    getPlaybackState: builder.query<PlaybackState, void>({
      query: () => "/playback",
      onCacheEntryAdded: async (
        arg,
        { cacheDataLoaded, cacheEntryRemoved, updateCachedData }
      ) => {
        const listener = (data: PlaybackState) => {
          updateCachedData(() => {
            return data;
          });
        };
        try {
          await cacheDataLoaded;
          eventSource.on("playback", listener);
        } catch (e) {}
        await cacheEntryRemoved;
        eventSource.removeListener("playback", listener);
      },
    }),
  }),
});

export const { useGetPlaybackStateQuery } = playbackApi;
