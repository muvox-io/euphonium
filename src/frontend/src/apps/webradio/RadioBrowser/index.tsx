import { useEffect, useState } from "preact/hooks";
import PlaybackAPI from "../../../api/euphonium/playback/PlaybackAPI";
import { getStationsByName } from "../../../api/radiobrowser/api";
import { Station } from "../../../api/radiobrowser/models";
import Card from "../../../components/ui/Card";
import Icon from "../../../components/ui/Icon";
import useAPI from "../../../utils/useAPI.hook";

const Radio = ({
  name = "",
  codec = "",
  favicon = "",
  bitrate = 0,
  countrycode = "",
  url_resolved = "",
}) => {
  const playbackAPI = useAPI(PlaybackAPI);

  return (
    <div class="bg-app-primary p-5 flex flex-col relative rounded-xl">
      <img
        class="w-10 h-10 rounded-full bg-white border-app-border border absolute -right-2 -top-2 shadow-xl object-contain"
        src={
          favicon ||
          "https://www.veryicon.com/download/png/media/music-series/sound-wave-1-2?s=256"
        }
      />
      <div class="font-normal max-w-[80%] -mt-1 mb-2 truncate">{name}</div>
      <div class="font-regular text-sm truncate text-gray-400">
        {codec} Codec • {bitrate} kbps • {countrycode}
      </div>
      <div class="bg-green-600 active:bg-green-800 text-white w-10 h-10 rounded-full absolute flex -bottom-2 -right-2 cursor-pointer">
        <div class="mt-[9px] ml-[5px]">
          <Icon
            onClick={() =>
              playbackAPI.playRadio(name, favicon, url_resolved)
            }
            name="play"
          />
        </div>
      </div>
    </div>
  );
};

export default () => {
  const pageSize = 30;

  const [radios, setRadios] = useState<Station[]>([]);
  const [page, setPage] = useState(0);

  return (
    <div class="mb-[150px]">
      <Card title="Web radio" subtitle="application">
        <div class="min-w-full relative mt-5">
          <input
            placeholder={"Search radios"}
            className="pl-10 bg-app-primary h-[45px] p-3 rounded-xl min-w-full"
            onChange={(e: any) =>
              getStationsByName(e.target.value, pageSize, page * pageSize).then(setRadios)
            }
          ></input>
          <div class="text-app-text-secondary left-1 top-3 absolute">
            <Icon name="search" />
          </div>
        </div>
        {/* <Input
          onSubmit={(e) => {
            getStationsByName(e).then(setRadios);
          }}
          value=""
          placeholder="Search radio-browser.info"
          icon="search"
        /> */}
        {radios.length > 0 && (
          <div class="grid md:grid-cols-2 grid-cols-1 xl:grid-cols-3 gap-8 mt-5">
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
        <div class="flex flex-row text-2xl">
          <div class={`${page > 0 ? 'bg-app-primary' : ''} text-app-text-secondary w-[40px] h-[40px] rounded-xl mr-3 flex justify-center items-center`}>
            <Icon name="arrow_right" />
          </div>

          <div class="text-app-text-secondary text-[18px] bg-app-primary w-[40px] h-[40px] rounded-xl mr-3 flex justify-center items-center">
            1/1
          </div>
          <div class="text-app-text-secondary bg-app-primary w-[40px] h-[40px] rounded-xl mr-3 flex justify-center items-center">
            <Icon name="arrow_left" />
          </div>
        </div>
      </Card>
    </div>
  );
};
