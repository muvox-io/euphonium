import css from "./App.module.scss";

import SideBar from '../SideBar'
import ConfiguratorCard from '../ConfiguratorCard'
import Playback from "../Playback";
import Light from "../../theme/Light";
import Dark from "../../theme/Dark";
import Router, { Route } from "preact-router";
import "../../theme/main.scss";

export function App() {
  return (
    <>
      <Dark/>
      <div className={css.mainWrapper}>
          <SideBar />
          <div className={css.mainWrapper__mainContent}>
            <Router>
              <Route path="/plugin/:plugin" component={ConfiguratorCard}/>
            </Router>
            <Playback/>
          </div>
      </div>
    </>
  )
}
