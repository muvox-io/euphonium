import { useEffect, useState } from "preact/hooks";
import { playRadio } from "../../../api/euphonium/api";
import { getStationsByName } from "../../../api/radiobrowser/api";
import { Station } from "../../../api/radiobrowser/models";
import Icon from "../../../components/Icon";
import Input from "../../../components/Input";
import css from "./RadioBrowser.module.scss";

const Radio = ({
  name = "",
  codec = "",
  favicon = "",
  bitrate = 0,
  countrycode = "",
  url_resolved = ""
}) => {
  return (
    <div className={css.stationContainer}>
      <img
        className={css.stationContainer__cover}
        src={
          favicon ||
          "https://www.veryicon.com/download/png/media/music-series/sound-wave-1-2?s=256"
        }
      />
      <div className={css.stationContainer__name}>{name}</div>
      <div className={css.stationContainer__subheader}>
        {codec} Codec • {bitrate} kbps • {countrycode}
      </div>
      <div className={css.stationContainer__controls}>
          <Icon onClick={() => playRadio(url_resolved, codec)} name="play" />
        <Icon name="save" />
      </div>
    </div>
  );
};

export default () => {
  const [radios, setRadios] = useState<Station[]>([]);

  return (
    <div>
      <Input
        onChange={(e) => {
          getStationsByName(e).then(setRadios);
        }}
        value=""
        placeholder="Search radio-browser.info"
        icon="search"
      />
      <div className={css.savedTitle}>Saved stations</div>
      <div className={css.stationList}>
        {radios.map((radio) => (
          <Radio {...radio}></Radio>
        ))}
      </div>
    </div>
  );
};
