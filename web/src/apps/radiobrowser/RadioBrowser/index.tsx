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
  url_resolved = "",
}) => {
  return (
    <div class="bg-app-secondary shadow-m p-5 border border-app-border flex flex-col relative rounded-xl">
      <img
        class="w-10 h-10 rounded-full bg-white border-app-border border absolute -right-2 -top-2 shadow-xl object-contain"
        src={
          favicon ||
          "https://www.veryicon.com/download/png/media/music-series/sound-wave-1-2?s=256"
        }
      />
      <div class="font-normal max-w-[80%] -mt-1 mb-2 truncate">{name}</div>
      <div class="font-thin text-sm truncate text-gray-400">
        {codec} Codec • {bitrate} kbps • {countrycode}
      </div>
      <div class="bg-green-600 active:bg-green-800 text-white w-10 h-10 rounded-full absolute flex -bottom-2 -right-2 cursor-pointer">
        <div class="mt-[9px] ml-[5px]">
          <Icon
            onClick={() => playRadio(name, favicon, url_resolved, codec)}
            name="play"
          />
        </div>
        {/* <Icon name="save" /> */}
      </div>
    </div>
  );
};

export default () => {
  const [radios, setRadios] = useState<Station[]>([]);

  return (
    <div class="mb-[150px]">
      <Card title="Web radio" subtitle="application">
        <Input
          onSubmit={(e) => {
            getStationsByName(e).then(setRadios);
          }}
          value=""
          placeholder="Search radio-browser.info"
          icon="search"
        />
        {radios.length > 0 && (
          <div class="grid md:grid-cols-2 grid-cols-1 xl:grid-cols-5 gap-8 mt-10">
            {radios.map((radio) => (
              <Radio {...radio}></Radio>
            ))}
          </div>
        )}
        {radios.length == 0 && (
          <div class="text-app-text-secondary text-xl mt-2 flex-col flex w-full items-center p-5">
              <Icon name="close" />
              Results empty.
          </div>
        )}
      </Card>
    </div>
  );
};
