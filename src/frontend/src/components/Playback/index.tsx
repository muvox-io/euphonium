import { useState } from "preact/hooks";
import {
  PlaybackState,
  PlaybackStatus,
} from "../../api/euphonium/playback/models";
import PlaybackAPI from "../../api/euphonium/playback/PlaybackAPI";
import useAPI from "../../utils/useAPI.hook";
import usePlaybackState from "../../utils/usePlaybackState.hook";
import Icon from "../ui/Icon";
import { useDispatch } from "react-redux";
import { onVolumeChange } from "../../redux/reducers/localPlaybackState";
import Spinner from "../ui/Spinner";

interface PlaybackStateProps {
  playbackState?: PlaybackState;
}

const PlaybackImage = ({ playbackState }: PlaybackStateProps) => {
  if (!playbackState?.track?.iconUrl) return <></>;
  return (
    <div>
      <div class="absolute bg-gray-500 w-[80px] h-[80px] left-6 bottom-2 rounded-xl">
        <img
          src={playbackState?.track?.iconUrl}
          class="rounded-xl w-full h-full bg-white object-contain"
        ></img>
      </div>
      <div class="w-[90px]"></div>
    </div>
  );
};

const PlaybackSong = ({ playbackState }: PlaybackStateProps) => {
  const track = playbackState?.track;
  if (!track?.title) {
    return <div class="text-white ml-4">Queue empty</div>;
  }

  return (
    <div class="text-white ml-4">
      {track?.title}{" "}
      <span class="text-app-text-secondary">via {track?.source}</span>
    </div>
  );
};

const PlaybackStatusControl = (playbackStatus: PlaybackStatus) => {
  const playbackAPI = useAPI(PlaybackAPI);

  switch (playbackStatus) {
    case PlaybackStatus.Playing:
      return (<Icon
        onClick={() => {
          playbackAPI.setPaused(true);
        }} name="pause" />)
    case PlaybackStatus.Paused:
      return (<Icon
        onClick={() => {
          playbackAPI.setPaused(false);
        }} name="play" />)
    case PlaybackStatus.Loading:
      return <span class="opacity-60"><Icon name="pause"/></span>;
    case PlaybackStatus.EmptyQueue:
      return <></>;
    default:
      return <></>;
  }
};

interface VolumeSliderProps {
  volume: number;
  onVolumeChange: (volume: number) => void;
  min?: number;
  max?: number;
  scrollStep?: number;
}

/**
 * A dumb component for the volume slider. It doesn't handle any state, it just
 * renders the slider and immediately calls the onVolumeChange callback when
 * the slider is moved or the scroll wheel is used on it.
 * @param _props
 * @returns
 */
const VolumeSlider = (_props: VolumeSliderProps) => {
  const props = {
    min: 0,
    max: 100,
    ..._props,
    scrollStep: _props.scrollStep || 5,
  };
  return (
    <>
      <Icon name="volume-up" />
      <input
        class="w-[100px]"
        type="range"
        id="volume"
        name="volume"
        min={props.min}
        max={props.max}
        onMouseUp={({ target }: any) =>
          props.onVolumeChange(parseFloat(target?.value))
        }
        onInput={({ target }: any) =>
          props.onVolumeChange(parseFloat(target?.value))
        }
        value={props.volume}
        onWheel={({ target, deltaY }: any) => {
          let delta = deltaY > 0 ? -1 : 1;
          delta *= props.scrollStep;
          const newVolume = Math.min(
            Math.max(props.volume + delta, props.min),
            props.max
          );
          if (newVolume != props.volume) {
            props.onVolumeChange(newVolume);
          }
        }}
      />
      <div class="text-sm ml-3 w-7 mr-1">{props.volume}%</div>
    </>
  );
};

const PlaybackControls = ({ playbackState }: PlaybackStateProps) => {
  const playbackAPI = useAPI(PlaybackAPI);
  return (
    <div class="mr-2">

      {PlaybackStatusControl(playbackState?.settings?.state ?? PlaybackStatus.EmptyQueue)}
    </div>
  );
};

const PlaybackBar = ({ themeColor = "#fff" }: { themeColor?: string }) => {
  const playbackState = usePlaybackState();
  const dispatch = useDispatch();
  return (
    <div class="flex flex-col flex-grow bottom-0 fixed right-0 left-0 md:left-[220px] h-[50px]">
      <div class="flex text-app-text-secondary flex-row bg-app-secondary border border-app-border md:bg-app-primary h-13 mr-3 ml-3 rounded-t-xl items-center shadow-xl flex-grow">
        <PlaybackImage playbackState={playbackState} />
        <PlaybackSong playbackState={playbackState} />
        <div class="ml-auto"></div>
        <VolumeSlider
          volume={playbackState.localVolume}
          onVolumeChange={(volume) => {
            console.log("onVolumeChange", volume);
            dispatch(onVolumeChange(volume) as any);
          }}
        />

        <PlaybackControls playbackState={playbackState} />
      </div>
    </div>
  );
};

export default () => {
  return <PlaybackBar />;
};
