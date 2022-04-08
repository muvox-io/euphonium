import { useContext } from "preact/hooks";
import { PlaybackState } from "../api/euphonium/playback/models";
import { PlaybackDataContext } from "./PlaybackContext";

export default function usePlaybackState(): PlaybackState {
  return useContext(PlaybackDataContext);
}
