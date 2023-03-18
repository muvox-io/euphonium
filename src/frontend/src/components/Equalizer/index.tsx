import { useState } from "preact/hooks";
import { EqSettings } from "../../api/euphonium/playback/models";
import PlaybackAPI from "../../api/euphonium/playback/PlaybackAPI";
import useAPI from "../../utils/useAPI.hook";

interface IEqBandParams {
  type: string;
  value: number;
  onValueChanged: (a: number, blur: boolean) => void;
}

const EqBand = ({ type, onValueChanged, value }: IEqBandParams) => {
  const [bandValue, setBandValue] = useState<number>(value);

  return (
    <div class="flex flex-col">
      <input
        class="w-[100px] transform -rotate-90 origin-left mt-[100px] mr-[-50px]"
        onInput={(e) => {
          const value = (e.target as any).value / 2;
          setBandValue(value - 4);
          onValueChanged((value - 4 ), false);
        }}
        onBlur={(e) => {
          const value = (e.target as any).value / 2;
          setBandValue(value - 4);
          onValueChanged((value - 4 ), true);
        }}
        type="range"
        id="volume"
        name="volume"
        value={(value + 4) * 2}
        min="0"
        max="16"
      />
      <div class="text-xs -ml-10">
        <p>
          {bandValue >= 0 ? "+" : ""}
          {bandValue * 3}db
        </p>
        <p class="text-thin mt-1">{type}</p>
      </div>
    </div>
  );
};

interface IEqualizerProps {
    eq: EqSettings
}

export default ({ eq }: IEqualizerProps) => {
  const [eqSettings, setEqSettings] = useState<EqSettings>(eq);
  const api = useAPI(PlaybackAPI);

  const updateSettings = (value: any, type: any, persist: boolean) => {
    const newSettings = { ...eqSettings, [type]: value };
    setEqSettings(newSettings);
    api.updateEq(newSettings, persist);
  };
  return (
      <div class="flex flex-row mt-2 ml-4">
        <EqBand
          type="bass"
          value={eqSettings["low"]}
          onValueChanged={(val, blur) => updateSettings(val, "low", blur)}
        />

        <EqBand
          type="mid"
          value={eqSettings["mid"]}
          onValueChanged={(val, blur) => updateSettings(val, "mid", blur)}
        />

        <EqBand
          type="treble"
          value={eqSettings["high"]}
          onValueChanged={(val, blur) => updateSettings(val, "high", blur)}
        />
      </div>
  );
};
