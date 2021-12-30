import { createContext } from "preact";
import { useState } from "preact/hooks";

const PlaybackDataContext = createContext<any>({});

const PlaybackDataContextProvider = ({ children }: any) => {
  const [playbackState, setPlaybackState] = useState({});

  return (
    <PlaybackDataContext.Provider value={{ playbackState, setPlaybackState }}>
      {...children}
    </PlaybackDataContext.Provider>
  );
};

export { PlaybackDataContext, PlaybackDataContextProvider };
