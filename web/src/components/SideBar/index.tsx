import { Link } from "preact-router/match";
import { useState, useEffect } from "preact/hooks";
import { getPlugins } from "../../api/euphonium/api";
import { PluginEntry, PluginEntryType } from "../../api/euphonium/models";
import Icon from "../Icon";
import SelectItem from "../SelectItem";

const SideBarItem = ({ displayName = "", name = "", type = ""}) => {
  return (
    <SelectItem>
      <Link
        activeClassName={""}
        href={type == "app" ? `/web/apps/${name}` : `/web/plugin/${name}`}
      >
        <div className={""}>{displayName}</div>
      </Link>
    </SelectItem>
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
      <div className="-mt-1 text-lg text-app-text-secondary md:text-sm">{header}</div>

      {plugins
        .filter((e) => e.type == filterType)
        .map((result) => (
          <SideBarItem {...result} />
        ))}
    </div>
  );
};

export default ({ version = "", theme = "", onThemeChange = () => {} }) => {
  const [plugins, setPlugins] = useState<PluginEntry[]>([]);

  useEffect(() => {
    getPlugins().then((data) => {
      setPlugins(data);
    });
  }, []);

  return (
    <div className="flex align-start relative md:w-[220px] md:min-w-[220px] flex-col bg-app-primary p-8 md:p-4 h-screen text-m space-y-2 overflow-y-auto">
      <div className="text-3xl md:text-2xl">Euphonium 🎺</div>
      <SideBarCategory
        plugins={plugins}
        filterType={PluginEntryType.App}
        header="available apps"
      />
      <SideBarCategory
        plugins={plugins}
        filterType={PluginEntryType.System}
        header="system configuration"
      />
      <SideBarCategory
        plugins={plugins}
        filterType={PluginEntryType.Plugin}
        header="plugin configuration"
      />


    <div class="absolute bottom-4 left-4 text-xs">{version}</div>
      <div onClick={(v) => onThemeChange() } class="absolute bg-app-secondary rounded-full w-8 h-8 right-4 bottom-4 flex text-xl items-center justify-center cursor-pointer">
        <Icon name="moon"/>
      </div>
    </div>
  );
};
