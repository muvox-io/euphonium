import { Connectivity, EuphoniumInfo } from "../../api/euphonium/system/models";
import SystemAPI from "../../api/euphonium/system/SystemAPI";
import "../../index.css";
import "../../theme/main.scss";
import { PlaybackDataContextProvider } from "../../utils/PlaybackContext";
import APIFetcher from "../APIFetcher";
import ConnectionLostModal from "../ConnectionLostModal";
import Onboarding from "../NetworkConfig";
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
      <APIFetcher api={SystemAPI} fetch={(api) => api.getSystemInfo()}>
        {(info: EuphoniumInfo) => {
          const onboarding = info?.connectivity?.state != "CONNECTED" || info?.onboarding;
          return (
            <div className={`${css.mainWrapper} ${onboarding ? css.mainWrapperWaves : css.mainWrapperStatic}`}>
              <ConnectionLostModal></ConnectionLostModal>
              <PlaybackDataContextProvider>
                <div class="h-screen w-screen">
                  <Notifications />
                  {/* <Onboarding/> */}
                  {onboarding ?
                    (<Onboarding connectivity={info.connectivity} />)
                    : (<NormalComponent info={info} />)
                  }
                </div>
              </PlaybackDataContextProvider>
            </div>
          );
        }}
      </APIFetcher>
    </>
  );
}
