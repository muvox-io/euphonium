import Router, { Route } from "preact-router";
import { EuphoniumInfo } from "../../api/euphonium/system/models";
import RadioBrowser from "../../apps/webradio/RadioBrowser";
import useIsMobile from "../../utils/isMobile.hook";
import ConfiguratorCard from "../ConfiguratorCard";
import ErrorBoundaryWrapper from "../ErrorBoundaryWrapper";
import Playback from "../Playback";
import PlaybackMobile from "../PlaybackMobile";
import SideBar from "../SideBar";
import SplashScreen from "../SplashScreen";

export default function NormalComponent({ info }: { info: EuphoniumInfo }) {
  const updateTheme = () => {
    const isLight = localStorage.getItem("theme") == "light";
    document.documentElement.className = isLight ? "themeLight" : "themeDark";
  };

  updateTheme();

  const toggleTheme = () => {
    const isLight = localStorage.getItem("theme") == "light";
    localStorage.setItem("theme", isLight ? "dark" : "light");
    updateTheme();
  };
  const isMobile = useIsMobile();
  return (
    <div class="flex-row flex">
      {!isMobile ? (
        <SideBar version={info?.version} onThemeChange={() => toggleTheme()} />
      ) : null}
      <div class="flex-grow h-screen overflow-y-auto pb-20">
        <Router>
          {isMobile ? <Route path="/web" component={SideBar} /> : null}
          <Route
            path="/web/plugin/:plugin"
            component={ErrorBoundaryWrapper(ConfiguratorCard)}
          />
          <Route path="/web" component={ErrorBoundaryWrapper(SplashScreen)} />

          <Route
            path="/web/apps/radio"
            component={ErrorBoundaryWrapper(RadioBrowser)}
          />
          <Route
            path="/web/playback"
            component={ErrorBoundaryWrapper(PlaybackMobile)}
          />
        </Router>
        <Playback />
      </div>
    </div>
  );
}
