import { DACPreset } from "./models";

const getDACPresets = async (): Promise<DACPreset[]> => {
  return await fetch(
    `https://raw.githubusercontent.com/feelfreelinux/euphonium/master/dacs.json`
  ).then((e) => e.json());
};

export { getDACPresets };
