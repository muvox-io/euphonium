import WebRadioWidget from "../../apps/webradio/WebRadioWidget";
import Icon from "../ui/Icon";
import IconCard from "../ui/IconCard";
import RecentlyPlayedCard from "./RecentlyPlayed";

interface PlaybackItem {
  title: string;
  subtitle: string;
  canPlay: boolean;
}

var playbackItems: PlaybackItem[] = [
  { title: 'Radio 357 AAC', subtitle: 'via radio, 2m ago', canPlay: true },
  { title: 'badradio 24/7 FR', subtitle: 'via radio, 10m ago', canPlay: true },
  { title: 'Avril 14th, Aphex Twin', subtitle: 'via bluetooth, 1h ago', canPlay: false }
]

const HardwareSectionCard = () => {
  return (<IconCard iconName="speaker" label="Hardware">
    <div class="flex flex-row">
      <div class="flex-col flex self-stretch">
        <div class="text-lg">μVox</div>

        <div class="text-sm text-app-text-secondary">2x80W smart amplifier</div>
        <div class="text-sm text-app-text-secondary mt-auto">v0.9, Feb 2023</div>
        <div class="text-sm text-app-text-secondary">SN AB2TXM</div>
        <div class="text-sm text-green-300 mt-1">device is officially supported</div>
      </div>
      <div class="bg-app-secondary rounded-xl p-6 ml-auto -mt-8">
        <img class="w-[160px] align-center" src="https://raw.githubusercontent.com/muvox-io/euphonium/f9676280584a502628a40afb9dca9f1a92297c36/docs/assets/boards/icon-muvox.svg" />
      </div>
    </div>
  </IconCard>);
}

const PlaybackCover = ({ uri }: any) => {
}

const Widgets = () => {
  return (
    <div class="flex flex-col md:flex-row">
      <div class="flex flex-col md:w-[50%] md:pr-4">
        <RecentlyPlayedCard />
      </div>
      <div class="flex flex-col md:w-[50%] md:pl-4">
        <HardwareSectionCard/>
      </div>
    </div>
  );
};

const Header = () => { };

export default () => {
  return (
    <div class="p-8 -mt-8 flex flex-col">
      <Widgets />
    </div>
  );
};

