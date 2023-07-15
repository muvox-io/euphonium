import { useEffect } from "preact/hooks";
import PlaybackAPI from "../../../api/euphonium/playback/PlaybackAPI";
import { Station } from "../../../api/radiobrowser/models";

import Card from "../../../components/ui/Card";
import Icon from "../../../components/ui/Icon";
import useAPI from "../../../utils/useAPI.hook";
import { useDispatch, useSelector } from "react-redux";
import { RadioBrowserReducerState, StationInReducer, searchStationsByName } from "../../../redux/reducers/radiobrowserReducer";
import { RootState } from "../../../redux/store";
import { radioApiEndpoints, usePlayRadioMutation, useMarkStationFavoriteMutation } from "../../../redux/api/radiobrowser/radioApi";

const Radio = (station: StationInReducer) => {
  const dispatch = useDispatch();
  const [triggerFavorite] = useMarkStationFavoriteMutation();
  const [playRadio] = usePlayRadioMutation();

  const { name, favicon, bitrate, countrycode, codec } = station;

  return (
    <div class="bg-app-primary cursor-pointer overflow-hidden h-[75px] w-auto flex flex-row rounded-xl" >
      <img
        class="h-[75px] w-[75px] bg-white rounded-l-xl bg-white border-app-border border shadow-xl object-cover"
        hidden={!favicon}
        onError={(e: any) => {
          e.target.style.display = "none";
        }}
        src={
          favicon ||
          "https://www.veryicon.com/download/png/media/music-series/sound-wave-1-2?s=256"
        }
      />

      <div class="flex flex-col relative justify-center pl-4 w-full truncate mr-4">
        <div class="font-normal -mt-1 mb-2 truncate">{name || 'Unknown station'}</div>
        <div class="font-regular text-sm text-gray-400 truncate">
          {codec} Codec • {bitrate > 0 ? `${bitrate} kbps • ` : ""} {countrycode}
        </div>
      </div>
      <div class="flex flex-col border-l border-app-border w-[38px]">
        <div class={`h-full bg-green-500 hover:opacity-80 border-app-border flex`}>
          <Icon
            onClick={() => {
              playRadio({ url: station.url, title: station.name, iconUrl: station.favicon })
            }}
            name="play"
          />
        </div>
        <div class={`h-full border-t hover:opacity-80 border-app-border ${station.favorite ? "bg-app-accent" : "text-gray-400 bg-app-primary"} flex`}>
          <Icon
            onClick={() => {
              station.favorite = !station.favorite;
              triggerFavorite(station);
            }}
            name={station.favorite ? "heart" : "heart"}
          />
        </div>
      </div>
    </div>
  );
};

const drawRadios = (radios: StationInReducer[], isFavorites: boolean) => {
  const title = isFavorites ? "Favorite stations" : "Results";
  return (
    <><div class="text-app-text-secondary mt-3">{title}</div>
      {radios.length == 0 && (
        <div class="text-app-text-secondary text-xl mt-2 flex-col flex w-full items-center p-5">
          {!isFavorites && (<Icon name="close" />)}
          Results empty.
        </div>
      )}

      {radios.length > 0 && (
        <div class="grid md:grid-cols-2 grid-cols-1 xl:grid-cols-3 gap-8 mt-3">
          {radios.map((radio) => (
            <Radio {...radio}></Radio>
          ))}
        </div>
      )}
    </>
  )
}

export default () => {
  const dispatch = useDispatch();

  const {
    favorites,
    results,
    isSearching
  } = useSelector<RootState>((state) => state.radiobrowser) as RadioBrowserReducerState;

  useEffect(() => {
    dispatch(radioApiEndpoints.getFavoriteStations.initiate() as any);
  }, [dispatch]);

  return (
    <div class="mb-[150px]">
      <Card title="Web radio" subtitle="application">
        <div class="min-w-full relative mt-5">
          <input
            placeholder={"Search radios"}
            className="pl-10 bg-app-primary h-[45px] p-3 rounded-xl min-w-full"
            onChange={(e: any) => {
              dispatch(searchStationsByName(e.target.value) as any);
            }}
          ></input>
          <div class="text-app-text-secondary left-1 top-3 absolute">
            <Icon name="search" />
          </div>
        </div>
        {drawRadios(isSearching ? results : favorites, !isSearching)}
      </Card>
    </div>
  );
};
