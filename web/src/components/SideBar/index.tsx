import { Link } from "preact-router/match";
import { useState, useEffect } from "preact/hooks";
import { getPlugins } from "../../api/euphonium/api";
import { PluginEntry, PluginEntryType } from "../../api/euphonium/models";
import SelectItem from "../SelectItem";

const SideBarItem = ({ displayName = "", name = "", type = "" }) => {
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
    <div className="flex flex-col space-y-4">
      <div className="text-gray-400 text-lg md:text-sm">{header}</div>

      {plugins
        .filter((e) => e.type == filterType)
        .map((result) => (
          <SideBarItem {...result} />
        ))}
    </div>
  );
};

export default () => {
  const [plugins, setPlugins] = useState<PluginEntry[]>([]);

  useEffect(() => {
    getPlugins().then((data) => {
      setPlugins(data);
    });
  }, []);

  return (
    <div className="flex md:w-[220px] md:min-w-[220px] flex-col bg-gray-700 p-8 md:p-4 h-screen text-m space-y-2 overflow-y-auto">
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
    </div>
  );
};
