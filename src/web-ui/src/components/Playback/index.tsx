import { useEffect, useState } from "preact/hooks";
import eventSource from "../../api/euphonium/eventSource";
import {
  PlaybackState,
  PlaybackStatus,
} from "../../api/euphonium/playback/models";
import PlaybackAPI from "../../api/euphonium/playback/PlaybackAPI";
import useIsMobile from "../../utils/isMobile.hook";
import useAPI from "../../utils/useAPI.hook";
import usePlaybackState from "../../utils/usePlaybackState.hook";
import APIFetcher from "../APIFetcher";
import Equalizer from "../Equalizer";
import Modal from "../Modal";
import Icon from "../ui/Icon";

export function debounce<T extends unknown[], U>(
  callback: (...args: T) => PromiseLike<U> | U,
  wait: number
) {
  let timer: number;

  return (...args: T): Promise<U> => {
    clearTimeout(timer);
    return new Promise((resolve) => {
      timer = setTimeout(() => resolve(callback(...args)), wait);
    });
  };
}

interface PlaybackStateProps {
  playbackState?: PlaybackState;
}

const PlaybackImage = ({ playbackState }: PlaybackStateProps) => {
  if (!playbackState?.track?.iconUrl) return (<></>)
  return (<div><div class="absolute bg-gray-500 w-[80px] h-[80px] left-6 bottom-2 rounded-xl">
    <img src={playbackState?.track?.iconUrl} class="rounded-xl"></img>

  </div><div class="w-[90px]"></div></div>)

}

const PlaybackSong = ({ playbackState }: PlaybackStateProps) => {
  const track = playbackState?.track;
  if (!track?.name) {
    return (<div class="text-white ml-4">
      Queue empty
    </div>);
  }

  return (<div class="text-white ml-4">{track?.name} <span class="text-app-text-secondary font-thin">via radio</span></div>)
}

const PlaybackVolume = ({ playbackState }: PlaybackStateProps) => {
  const [volume, setVolume] = useState(0);
  return (<>
    <Icon name="volume-up" />
    <input
      class="w-[100px]"
      type="range"
      id="volume"
      name="volume"
      min="0"
      max="100"
    />
    <div class="text-sm ml-3">{volume}%</div>
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
  const playbackAPI = useAPI(PlaybackAPI);
  const playbackState = usePlaybackState();

  const [eqOpen, setEqOpen] = useState<boolean>(false);
  const [mobileDialogOpen, setMobileDialogOpen] = useState<boolean>(false);
  const [volume, updateVolume] = useState(0);

  const volUpdated = (volume: number, persist: boolean) => {
    // playbackAPI.updateVolume(Math.round((volume / 15) * 100), persist);
    updateVolume(volume);
  };

  /*const volUpdated = debounce(
    (volume: number, persist: boolean = false) =>
      volUpdatedInstant(volume, persist),
    100
  );*/

  const isMobile = useIsMobile();

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
