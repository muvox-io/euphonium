import { EuphoniumInfo } from "../../api/euphonium/system/models";
import SystemAPI from "../../api/euphonium/system/SystemAPI";
import "../../index.css";
import "../../theme/main.scss";
import { PlaybackDataContextProvider } from "../../utils/PlaybackContext";
import APIFetcher from "../APIFetcher";
import ConnectionLostModal from "../ConnectionLostModal";
import Notifications from "../ui/Notifications";
import css from "./App.module.scss";
import NormalComponent from "./NormalComponent";
import RecoveryComponent from "./RecoveryComponent";
import WifiConfigurationComponent from "./WifiConfigurationComponent";

export function App() {
  const updateTheme = () => {
    const isLight = localStorage.getItem("theme") == "light";
    document.documentElement.className = isLight ? "themeLight" : "themeDark";
  };

  updateTheme();
  return (
    <>
      <div className={css.mainWrapper}>
        <ConnectionLostModal></ConnectionLostModal>
        <PlaybackDataContextProvider>
          <div class="md:bg-app-secondary bg-app-primary h-screen w-screen">
            <Notifications />
            <APIFetcher api={SystemAPI} fetch={(api) => api.getSystemInfo()}>
              {(info: EuphoniumInfo) => {
                switch (info?.networkState) {
                  case "recovery":
                    return <RecoveryComponent />;
                  case "online":
                    return <NormalComponent info={info} />;
                  case "offline":
                    return <WifiConfigurationComponent />;
                  default:
                    return (
                      <div>Unknown network state: {info?.networkState}</div>
                    );
                }
              }}
            </APIFetcher>
          </div>
        </PlaybackDataContextProvider>
      </div>
    </>
  );
}
