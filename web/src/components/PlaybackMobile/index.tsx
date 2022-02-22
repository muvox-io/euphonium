import {useState, useEffect} from "preact/hooks";
import {eventSource } from "../../api/euphonium/api";
import { getPlaybackState, PlaybackState, updateVolume } from "../../api/euphonium/playback";
import Card from "../Card";
import Equalizer from "../Equalizer";
import Icon from "../Icon";

export default function () {
  const [playbackState, setPlaybackState] = useState<PlaybackState>();

  useEffect(() => {
    getPlaybackState().then((e) => setPlaybackState(e));
  }, []);

  useEffect(() => {
    eventSource.addEventListener("playback", ({ data }: any) => {
      setPlaybackState(JSON.parse(data));
    });
  }, []);

  const [eqOpen, setEqOpen] = useState<boolean>(false);

  const volUpdated = (volume: number) => {
    updateVolume(Math.round((volume / 15) * 100));
  };
  return (
    <Card title="Playback" subtitle="Currently playing">
      <div class="flex flex-col items-center">
        <img src={playbackState?.song?.icon} class="bg-white w-[250px] h-[250px] rounded-xl border border-app-border" />
        <div>Danny</div>
        <div>Minecraft volume Alpha | C418</div>
        <div>
          <Icon name="vol-up" />
          <input
            class="w-20"
            type="range"
            id="volume"
            name="volume"
            value={8}
            min="0"
            max="15"
          />
        </div>

              <div class="text-center rounded-xl w-[170px] p-4 z-index-2 bg-app-secondary mb-1">
        </div>
      </div>
    </Card>
  );
}
