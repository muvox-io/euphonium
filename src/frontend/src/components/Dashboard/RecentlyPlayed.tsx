import { useEffect, useState } from "preact/hooks";
import { PlaybackTrack } from "../../api/euphonium/playback/models";
import PlaybackAPI from "../../api/euphonium/playback/PlaybackAPI";
import { timestampToHumanDuration } from "../../utils/humanDuration";
import useAPI from "../../utils/useAPI.hook";
import Icon from "../ui/Icon";
import IconCard from "../ui/IconCard";
import Separator from "../ui/Separator/Separator";

const RecentlyPlayedItem = ({ playbackItem }: { playbackItem: PlaybackTrack }) => {
  return (<div class="flex flex-row">
    <div class="flex flex-col text-sm">
      <div class="font-medium">{playbackItem?.title}</div>
      <div class="text-app-text-secondary">via {playbackItem?.source}, {timestampToHumanDuration(Date.now(), playbackItem?.timestamp)}</div>
    </div>
  </div>);
}

const RecentlyPlayedCard = ({ }) => {
  const playbackAPI = useAPI(PlaybackAPI);

  const [tracks, setTracks] = useState<PlaybackTrack[]>([]);

  useEffect(() => {
    playbackAPI.getRecentlyPlayed().then((resultTracks) => {
      setTracks(resultTracks);
    });
  }, []);

  return (<IconCard iconName="playlist" label="Recently played">
    <div class="space-y-3 mt-1">
      {tracks.map((track) => (<><RecentlyPlayedItem playbackItem={track} />{track != tracks[tracks.length - 1] ? <Separator /> : null}</>))}


    </div>
  </IconCard>);
}

export default RecentlyPlayedCard;
