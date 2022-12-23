import { Connectivity, EuphoniumInfo } from "../../api/euphonium/system/models";
import SystemAPI from "../../api/euphonium/system/SystemAPI";
import "../../index.css";
import "../../theme/main.scss";
import { PlaybackDataContextProvider } from "../../utils/PlaybackContext";
import APIFetcher from "../APIFetcher";
import ConnectionLostModal from "../ConnectionLostModal";
import Onboarding from "../Onboarding";
import Notifications from "../ui/Notifications";
import css from "./App.module.scss";
import NormalComponent from "./NormalComponent";

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
          <div class="h-screen w-screen">
            <Notifications />
            {/* <Onboarding/> */}
            <APIFetcher api={SystemAPI} fetch={(api) => api.getSystemInfo()}>
              {(info: EuphoniumInfo) => {
                if (info?.onboarding) {
                  return <Onboarding connectivity={info.connectivity}/>
                }

                return <NormalComponent info={info}/>
              }}
            </APIFetcher>
          </div>
        </PlaybackDataContextProvider>
      </div>
    </>
  );
}
