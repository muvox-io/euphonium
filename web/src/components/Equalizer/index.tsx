import { useState } from "preact/hooks";
import { updateEq } from "../../api/euphonium/api";
import {EqSettings} from "../../api/euphonium/models";

interface IEqBandParams {
  type: string;
  value: number;
  onValueChanged: (a: number) => void;
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
          onValueChanged((value - 4 ));
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

interface IEqualizerParams {
    eq: EqSettings
}

export default ({ eq }: IEqualizerParams) => {
  const [eqSettings, setEqSettings] = useState<EqSettings>(eq);

  const updateSettings = (value: any, type: any) => {
    const newSettings = { ...eqSettings, [type]: value };
    setEqSettings(newSettings);
    updateEq(newSettings);
  };
  return (
    <div>
      equalizer
      <div class="flex flex-row mt-2 ml-4">
        <EqBand
          type="bass"
          value={eqSettings["low"]}
          onValueChanged={(val) => updateSettings(val, "low")}
        />

        <EqBand
          type="mid"
          value={eqSettings["mid"]}
          onValueChanged={(val) => updateSettings(val, "mid")}
        />

        <EqBand
          type="treble"
          value={eqSettings["high"]}
          onValueChanged={(val) => updateSettings(val, "high")}
        />
      </div>
    </div>
  );
};
