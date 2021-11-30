import { useEffect, useState } from "preact/hooks";
import { playRadio } from "../../../api/euphonium/api";
import { getStationsByName } from "../../../api/radiobrowser/api";
import { Station } from "../../../api/radiobrowser/models";
import Card from "../../../components/Card";
import Icon from "../../../components/Icon";
import Input from "../../../components/Input";

const Radio = ({
  name = "",
  codec = "",
  favicon = "",
  bitrate = 0,
  countrycode = "",
  url_resolved = ""
}) => {
  return (
    <div class="bg-grey shadow-m p-5 flex flex-col relative rounded-xl">
      <img
        class="w-10 h-10 rounded-full bg-white absolute -right-2 -top-2"
        src={
          favicon ||
          "https://www.veryicon.com/download/png/media/music-series/sound-wave-1-2?s=256"
        }
      />
      <div class='font-normal max-w-[80%] -mt-1 mb-2 truncate'>{name}</div>
      <div class='font-thin text-sm truncate text-gray-400'>
        {codec} Codec • {bitrate} kbps • {countrycode}
      </div>
      <div class='bg-green-600 w-10 h-10 rounded-full absolute flex -bottom-2 -right-2'>
        <div class='mt-[9px] ml-[5px]'>
        <Icon onClick={() => playRadio(name, favicon, url_resolved, codec)} name="play" />
        </div>
        {/* <Icon name="save" /> */}
      </div>
    </div>
  );
};

export default () => {
  const [radios, setRadios] = useState<Station[]>([]);

  return (
    <Card title="Web radio" subtitle="application">
      <Input
        onChange={(e) => {
          getStationsByName(e).then(setRadios);
        }}
        value=""
        placeholder="Search radio-browser.info"
        icon="search"
      />
      <div class="grid md:grid-cols-2 grid-cols-1 xl:grid-cols-5 gap-8 mt-10">
        {radios.map((radio) => (
          <Radio {...radio}></Radio>
        ))}
      </div>
    </Card >
  );
};
