import { Link, Match } from "preact-router/match";
import { useState, useEffect } from "preact/hooks";
import { PluginEntry, PluginEntryType } from "../../api/euphonium/models";
import { getPlugins } from "../../api/euphonium/plugins";
import Icon from "../ui/Icon";
import SelectItem from "../ui/SelectItem";

const PluginIconMap = {
  'dashboard': 'home',
  'webradio': 'radio',
  'plugin': 'playlist',
  'general': 'settings',
  'dac': 'dac_settings'
} as { [key: string]: string };

const SideBarItem = ({ displayName = "", name = "", type = "" }) => {
  const hrefUrl = type == "app" ? `/web/apps/${name}` : `/web/plugin/${name}`;

  return (
    <Link
      activeClassName={""}
      href={hrefUrl}
    >
      <Match path={hrefUrl}>{({ matches, path, url }: any) =>
      (<div class={`text-m flex flex-row items-center ${ matches ? 'text-app-accent' : 'text-app-text-secondary ' }`}>
        <span class="text-2xl -ml-2 mr-1"><Icon name={PluginIconMap[name] ?? (PluginIconMap[type] ?? 'home')} /></span>

        <span class="font-thin">{displayName}</span>
        { matches ? <div class="w-1 h-[30px] bg-app-accent ml-auto -mr-4 rounded-l-lg"></div> : <></>}
      </div>)}
      </Match>
    </Link>
    // </SelectItem>
  );
};

type CategoryProps = {
  filterType: PluginEntryType;
  plugins: PluginEntry[];
  header: string;
};

const SideBarCategory = ({ plugins, filterType, header }: CategoryProps) => {
  return (
    <div className="flex flex-col space-y-3">
      <div className="mt-3 text-lg text-app-text-secondary md:text-sm font-semibold">{header}</div>

      {plugins
        .filter((e) => e.type == filterType)
        .map((result) => (
          <SideBarItem {...result} />
        ))}
    </div>
  );
};

export default ({ version = "", theme = "", onThemeChange = () => { } }) => {
  const [plugins, setPlugins] = useState<PluginEntry[]>([]);

  useEffect(() => {
    getPlugins().then((data) => {
      setPlugins([...data, { name: "general", displayName: "General", type: PluginEntryType.System }, { name: "dac", displayName: "DAC Settings", type: PluginEntryType.System }]);
    });
  }, []);

  return (
    <div className="flex align-start relative md:w-[220px] md:min-w-[220px] flex-col bg-app-primary p-8 md:p-4 h-screen text-m space-y-2 overflow-y-auto">
      <div className="text-3xl md:text-2xl">Euphonium</div>
      <div className="text-xs md:text-m text-app-text-secondary pb-3">tiny audio platform</div>
      <SideBarItem displayName="Dashboard" name="home"></SideBarItem>
      <SideBarCategory
        plugins={plugins}
        filterType={PluginEntryType.App}
        header="Applications"
      />
      <SideBarCategory
        plugins={plugins}
        filterType={PluginEntryType.System}
        header="System configuration"
      />
      <SideBarCategory
        plugins={plugins}
        filterType={PluginEntryType.Plugin}
        header="Plugin configuration"
      />

      <div class="absolute bottom-4 left-4 text-xs">{version}</div>
      <div onClick={(v) => onThemeChange()} class="absolute bg-app-secondary rounded-full w-8 h-8 right-4 bottom-4 flex text-xl items-center justify-center cursor-pointer">
        <Icon name="moon" />
      </div>
    </div>
  );
};
