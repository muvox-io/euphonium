import { PlaybackState } from "../api/euphonium/playback/models";
import { useGetPlaybackStateQuery } from "../redux/api/euphonium/playbackApi";

export default function usePlaybackState(): PlaybackState | undefined {
  const { data } = useGetPlaybackStateQuery();
  return data;
}
