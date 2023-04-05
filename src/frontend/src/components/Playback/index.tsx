import { useState } from "preact/hooks";
import {
  PlaybackState,
  PlaybackStatus
} from "../../api/euphonium/playback/models";
import PlaybackAPI from "../../api/euphonium/playback/PlaybackAPI";
import useAPI from "../../utils/useAPI.hook";
import usePlaybackState from "../../utils/usePlaybackState.hook";
import Icon from "../ui/Icon";


interface PlaybackStateProps {
  playbackState?: PlaybackState;
}

const PlaybackImage = ({ playbackState }: PlaybackStateProps) => {
  if (!playbackState?.track?.iconUrl) return (<></>)
  return (<div><div class="absolute bg-gray-500 w-[80px] h-[80px] left-6 bottom-2 rounded-xl">
    <img src={playbackState?.track?.iconUrl} class="rounded-xl w-full h-full bg-white object-contain"></img>

  </div><div class="w-[90px]"></div></div>)

}

const PlaybackSong = ({ playbackState }: PlaybackStateProps) => {
  const track = playbackState?.track;
  if (!track?.title) {
    return (<div class="text-white ml-4">
      Queue empty
    </div>);
  }

  return (<div class="text-white ml-4">{track?.title} <span class="text-app-text-secondary">via {track?.source}</span></div>)
}

const PlaybackVolume = ({ playbackState }: PlaybackStateProps) => {
  const playbackAPI = useAPI(PlaybackAPI);
  const [localVolume, setLocalVolume] = useState(0);

  if (playbackState?.settings?.volume && playbackState?.settings?.volume != localVolume) {
    setLocalVolume(playbackState?.settings?.volume);
  }

  const updateVolume = (volume: number, persist = false) => {
    if (persist) {
      playbackAPI.updateVolume(volume, persist = true);
    } else {
      playbackAPI.updateVolumeThrottled(volume);
    }
    setLocalVolume(volume);
    playbackState!.settings.volume = volume;
  }

  return (<>
    <Icon name="volume-up" />
    <input
      class="w-[100px]"
      type="range"
      id="volume"
      name="volume"
      onMouseUp={({ target }: any) => updateVolume(target?.value, true)}
      min="0"
      max="100"
      onInput={({ target }: any) => updateVolume(target?.value)}
      value={playbackState?.settings?.volume}
    />
    <div class="text-sm ml-3">{localVolume}%</div>
  </>)

}

const PlaybackControls = ({ playbackState }: PlaybackStateProps) => {
  const playbackAPI = useAPI(PlaybackAPI);
  return (<div class="mr-2"><Icon
    onClick={() => {
      playbackAPI.setPaused(
        playbackState?.settings?.state == PlaybackStatus.Playing
      );
    }}
    name={
      playbackState?.settings?.state == PlaybackStatus.Playing
        ? "pause"
        : "play"
    }
  /></div>)
}

const PlaybackBar = ({
  themeColor = "#fff",
}: {
  themeColor?: string;
}) => {
  const playbackState = usePlaybackState();


  const volUpdated = (volume: number, persist: boolean) => {
    // playbackAPI.updateVolume(Math.round((volume / 15) * 100), persist);
  };

  return (
    <div class="flex flex-col flex-grow bottom-0 fixed right-0 left-0 md:left-[220px] h-[50px]">
      <div class="flex text-app-text-secondary flex-row bg-app-secondary border border-app-border md:bg-app-primary h-13 mr-3 ml-3 rounded-t-xl items-center shadow-xl flex-grow">
        <PlaybackImage playbackState={playbackState} />
        <PlaybackSong playbackState={playbackState} />
        <div class="ml-auto"></div>
        <PlaybackVolume playbackState={playbackState} />

        <PlaybackControls playbackState={playbackState} />
      </div>
    </div >
  );
};

export default () => {
  return <PlaybackBar />;
};
