import WebRadioWidget from "../../apps/webradio/WebRadioWidget";
import IconCard from "../ui/IconCard";

const Widgets = () => {
  return (
    <div class="grid grid-cols-3 gap-4 items-start">
      <WebRadioWidget/>
    </div>
  );
};

const Header = () => {};

export default () => {
  return (
    <div class="p-8 flex flex-col">
      <div class="text-2xl">Welcome to Euphonium</div>
      <Widgets/>
    </div>
  );
};

