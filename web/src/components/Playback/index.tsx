import css from "./Playback.module.scss";
import { PlaybackState } from "../../api/euphonium/models";
import { eventSource, getPlaybackState } from "../../api/euphonium/api";
import Icon from "../Icon";
import { useEffect, useState } from "preact/hooks";

const PlaybackBar = () => {
  const [playbackState, setPlaybackState] = useState<PlaybackState>();

  useEffect(() => {
    getPlaybackState().then((e) => setPlaybackState(e));
  }, []);

  useEffect(() => {
    eventSource.addEventListener("playback", ({ data }: any) => {
      console.log(data);
      setPlaybackState(JSON.parse(data));
    });
  }, []);

  return (
    <div class='hidden flex flex-row bg-gray-700 h-13 rounded-r-xl items-center shadow-lg'>
      <img class='rounded-xl h-22 w-22 -mt-2 -mb-2'
        src={playbackState?.icon}
      ></img>
      <div class='flex flex-col justify-center ml-4'>
        <div >{playbackState?.songName}</div>
        <div class='text-gray-400'>
          {playbackState?.artistName} • {playbackState?.albumName} • playback
          from {playbackState?.sourceName}
        </div>
      </div>
      <div >
      </div>
    <div class='mr-3 text-xl text-gray-400 ml-5'>
      <Icon name="pause" />
    </div>
    </div>
  );
};

export default () => {
  return (
    <div class='fixed bottom-0 p-6 right-0' >
      <PlaybackBar />
    </div>
  );
};
