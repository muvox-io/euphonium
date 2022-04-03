import { createContext } from "preact";
import { useEffect, useState } from "preact/hooks";
import eventSource from "../api/euphonium/eventSource";
import PlaybackAPI from "../api/euphonium/playback/PlaybackAPI";
import useAPI from "./useAPI.hook";

const PlaybackDataContext = createContext<any>({});

const PlaybackDataContextProvider = ({ children }: any) => {
  const [playbackState, setPlaybackState] = useState({});

  const playbackAPI = useAPI(PlaybackAPI);
  const fetchInitialPlaybackState = async () => {
    const playbackState = await playbackAPI.getPlaybackState();
    setPlaybackState(playbackState);
  };
  useEffect(() => {
    fetchInitialPlaybackState();
  }, [playbackAPI]);
  useEffect(() => {
    const playbackStateChanged = ({ data }: any) => {
      setPlaybackState(JSON.parse(data));
    };
    eventSource.addEventListener("playback", playbackStateChanged);
    return () =>
      eventSource.removeEventListener("playback", playbackStateChanged);
  }, []);

  return (
    <PlaybackDataContext.Provider value={playbackState}>
      {...children}
    </PlaybackDataContext.Provider>
  );
};

export { PlaybackDataContext, PlaybackDataContextProvider };
