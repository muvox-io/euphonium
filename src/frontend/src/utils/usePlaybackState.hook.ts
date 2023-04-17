import { useSelector } from "react-redux";
import { useGetPlaybackStateQuery } from "../redux/api/euphonium/playbackApi";
import { LocalPlaybackState } from "../redux/reducers/localPlaybackState";
import { RootState } from "../redux/store";

export default function usePlaybackState(): LocalPlaybackState {
  useGetPlaybackStateQuery(); // force Redux RTK Query to fetch the data

  return useSelector((state: RootState) => state.localPlaybackState); // but return the local state instead of the fetched state
}
