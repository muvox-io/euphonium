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
    <div className={css.wrapper}>
      <img
        className={css.wrapper__cover}
        src={playbackState?.icon}
      ></img>
      <div className={css.wrapper__playbackContainer}>
        <div className={css.wrapper__songContainer}>
          <div className={css.wrapper__songName}>{playbackState?.songName}</div>
          <div className={css.wrapper__songSubtitle}>
            {playbackState?.artistName} • {playbackState?.albumName} • playback
            from {playbackState?.sourceName}
          </div>
        </div>
        <div className={css.wrapper__controlsContainer}>
          {/* <div className={css.wrapper__timeContainer}>2:00/2:40</div> */}
          {/* <Icon name="prev" /> */}
          <Icon name="pause" />
          {/* <Icon name="next" /> */}
        </div>
      </div>
    </div>
  );
};

const PlaybackProgress = () => {
  return (
    <div className={css.progressBar}>
      {/* <div className={css.progressBar__inner}></div> */}
    </div>
  );
};

export default () => {
  return (
    <div className={css.playbackContainer}>
      <PlaybackBar />
      <PlaybackProgress />
    </div>
  );
};
