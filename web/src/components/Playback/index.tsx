import css from "./Playback.module.scss";
import Icon from '../Icon';

const PlaybackBar = () => {
    return (<div className={css.wrapper}>
        <img
          className={css.wrapper__cover}
          src="https://upload.wikimedia.org/wikipedia/en/7/76/The_Smashing_Pumpkins_-_Mellon_Collie_And_The_infinite_Sadness.jpg"
        ></img>
        <div className={css.wrapper__playbackContainer}>
          <div className={css.wrapper__songContainer}>
            <div className={css.wrapper__songName}>Zero - Remastered 2012</div>
            <div className={css.wrapper__songSubtitle}>
              The Smashing Pumpkins • Mellon Collie And The Infinite Sadness •
              playback from Spotify (cspot)
            </div>
          </div>
          <div className={css.wrapper__controlsContainer}>
              <div className={css.wrapper__timeContainer}>
                  2:00/2:40
              </div>
              <Icon name="prev"/>
              <Icon name="pause"/>
              <Icon name="next"/>
          </div>
        </div>
      </div>)
}

const PlaybackProgress = () => {
    return (<div className={css.progressBar}>
        <div className={css.progressBar__inner}></div>
    </div>)
}


export default () => {
  return (
    <div className={css.playbackContainer}>
        <PlaybackBar/>
        <PlaybackProgress/>
    </div>
  );
};
