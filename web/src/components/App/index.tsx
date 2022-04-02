import Router, { Route } from "preact-router";
import { useEffect, useState } from "preact/hooks";
import { getInfo } from "../../api/euphonium/api";
import { EuphoniumInfo } from "../../api/euphonium/models";
import OTACard from "../../apps/ota/OTACard";
import RadioBrowser from "../../apps/webradio/RadioBrowser";
import WiFiConfigurator from "../../apps/wifi/WiFiConfigurator";
import "../../index.css";
import "../../theme/main.scss";
import useIsMobile from "../../utils/isMobile.hook";
import { PlaybackDataContextProvider } from "../../utils/PlaybackContext";
import ConfiguratorCard from "../ConfiguratorCard";
import ConnectionLostModal from "../ConnectionLostModal";
import Playback from "../Playback";
import PlaybackMobile from "../PlaybackMobile";
import SideBar from "../SideBar";
import SplashScreen from "../SplashScreen";
import Notifications from "../ui/Notifications";
import css from "./App.module.scss";

export function App() {
  const [info, setInfo] = useState<EuphoniumInfo>();

  const updateTheme = () => {
    const isLight = localStorage.getItem("theme") == "light";
    document.documentElement.className = isLight ? "themeLight" : "themeDark";
  };

  useEffect(() => {
    updateTheme();
    getInfo().then(setInfo);
  }, []);
  const isMobile = useIsMobile();

  updateTheme();

  const toggleTheme = () => {
    const isLight = localStorage.getItem("theme") == "light";
    localStorage.setItem("theme", isLight ? "dark" : "light");
    updateTheme();
  };

  return (
    <>
      <div className={css.mainWrapper}>
        <ConnectionLostModal></ConnectionLostModal>
        <PlaybackDataContextProvider>
          <div class="md:bg-app-secondary bg-app-primary h-screen w-screen">
            <Notifications />
            {info?.networkState == "recovery" ? <OTACard /> : null}
            {info?.networkState == "online" ? (
              <div class="flex-row flex">
                {!isMobile ? (
                  <SideBar
                    version={info?.version}
                    onThemeChange={() => toggleTheme()}
                  />
                ) : null}
                <div class="flex-grow h-screen overflow-y-auto">
                  <Router>
                    {isMobile ? (
                      <Route path="/web" component={SideBar} />
                    ) : null}
                    <Route
                      path="/web/plugin/:plugin"
                      component={ConfiguratorCard}
                    />
                    <Route path="/web" component={SplashScreen} />

                    <Route path="/web/apps/webradio" component={RadioBrowser} />
                    <Route path="/web/playback" component={PlaybackMobile} />
                  </Router>
                  <Playback />
                </div>
              </div>
            ) : info?.networkState == "offline" ? (
              <WiFiConfigurator />
            ) : null}
          </div>
        </PlaybackDataContextProvider>
      </div>
    </>
  );
}
