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

export function App() {
  const isMobile = useIsMobile();

  return (
    <>
      <Dark/>
      <div className={css.mainWrapper}>
        <div></div>
          { !isMobile ? <SideBar/> : null}
          <div className={css.mainWrapper__mainContent}>
            <Router>
              { isMobile ? <Route path="/web" component={SideBar} /> : null }
              <Route path="/web/plugin/:plugin" component={ConfiguratorCard}/>
              <Route path="/web/apps/webradio" component={RadioBrowser}/>
            </Router>
            <Playback/>
          </div>
      </div>
    </>
  )
}
