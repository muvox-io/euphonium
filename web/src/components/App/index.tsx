import css from "./App.module.scss";

import SideBar from '../SideBar'
import ConfiguratorCard from '../ConfiguratorCard'
import Light from "../../theme/Light";
import Dark from "../../theme/Dark";
import Router, { Route } from "preact-router";
import "../../theme/main.scss";

export function App() {
  return (
    <>
      <Light/>
      <div className={css.mainWrapper}>
          <SideBar />
          <Router>
            <Route path="/plugin/:plugin" component={ConfiguratorCard}/>
          </Router>
      </div>
    </>
  )
}
