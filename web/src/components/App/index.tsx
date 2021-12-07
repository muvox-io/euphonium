import css from "./App.module.scss";

import SideBar from '../SideBar'
import ConfiguratorCard from '../ConfiguratorCard'
import Playback from "../Playback";
import Light from "../../theme/Light";
import Dark from "../../theme/Dark";
import Router, { Route } from "preact-router";
import "../../theme/main.scss";
import RadioBrowser from "../../apps/radiobrowser/RadioBrowser";
import '../../index.css'
import useIsMobile from "../../utils/isMobile.hook";
import WiFiConfigurator from "../../apps/wifi/WiFiConfigurator";
import { useEffect, useState } from "preact/hooks";
import { EuphoniumInfo } from "../../api/euphonium/models";
import { getInfo } from "../../api/euphonium/api";

export function App() {
  const [info, setInfo] = useState<EuphoniumInfo>();
  const [isDark, setDark] = useState<boolean>(!(localStorage.getItem('theme') &&  localStorage.getItem('theme') == 'light'));

  useEffect(() => {
    getInfo().then(setInfo);
  }, []);
  const isMobile = useIsMobile();

  const toggleTheme = () => {
  }

  return (
    <>
      <Dark/>
      <div className={css.mainWrapper}>
        <div class='md:bg-app-secondary bg-app-primary h-screen w-screen'>
          {info?.networkState == 'online' ? (
            <div class="flex-row flex">
              {!isMobile ? <SideBar version={info?.version} onThemeChange={() => toggleTheme()} /> : null}
              <div class="flex-grow h-screen overflow-y-auto">
                <Router>
                  {isMobile ? <Route path="/web" component={SideBar} /> : null}
                  <Route path="/web/plugin/:plugin" component={ConfiguratorCard} />
                  <Route path="/web/apps/webradio" component={RadioBrowser} />
                </Router>
                <Playback />
              </div>
            </div>
          ) : (info?.networkState == 'offline' ? (<WiFiConfigurator />) : null)}
        </div>
      </div>
    </>
  )
}
