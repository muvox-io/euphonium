import { Link } from "preact-router/match";
import { useState, useEffect } from "preact/hooks";
import { getPlugins } from "../../api/euphonium/api";
import { PluginEntry, PluginEntryType } from "../../api/euphonium/models";
import css from "./SideBar.module.scss";

const SideBarItem = ({ displayName = "", name = "", type = "" }) => {
  return (
    <Link
      activeClassName={css.sideBar__itemselected}
      className={css.sideBar__itemwrapper}
      href={ type == "app" ? `/web/apps/${name}` : `/web/plugin/${name}` }
    >
      <div className={css.sideBar__item}>{displayName}</div>
    </Link>
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
    <div className={css.sideBar}>
      <div className={css.sideBar__header}>Euphonium 🎺</div>
      <div className={css.sideBar__subheader}>available apps</div>
      {plugins.filter((e) => e.type == PluginEntryType.App).map((result) => (
        <SideBarItem {...result} />
      ))}

      <div className={css.sideBar__subheader}>system modules</div>
      {plugins.filter((e) => e.type == PluginEntryType.System).map((result) => (
        <SideBarItem {...result} />
      ))}

      <div className={css.sideBar__subheader}>installed plugins</div>
      {plugins.filter((e) => e.type == PluginEntryType.Plugin).map((result) => (
        <SideBarItem {...result} />
      ))}
    </div>
  );
};
