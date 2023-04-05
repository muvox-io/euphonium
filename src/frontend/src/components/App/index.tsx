import "../../index.css";
import { useGetSystemInfoQuery } from "../../redux/api/euphonium/system/systemApi";
import "../../theme/main.scss";
import { PlaybackDataContextProvider } from "../../utils/PlaybackContext";
import ConnectionLostModal from "../ConnectionLostModal";
import Onboarding from "../NetworkConfig";
import ReduxAPIFetcher from "../ReduxAPIFetcher";
import Notifications from "../ui/Notifications";
import css from "./App.module.scss";
import NormalComponent from "./NormalComponent";

export function App() {
  const updateTheme = () => {
    const isLight = localStorage.getItem("theme") == "light";
    document.documentElement.className = isLight ? "themeLight" : "themeDark";
  };

  updateTheme();

  const result = useGetSystemInfoQuery();
  return (
    <>
      <ConnectionLostModal></ConnectionLostModal>
      <ReduxAPIFetcher result={result}>
        {({ data: info }) => {
          const onboarding =
            info?.connectivity?.state != "CONNECTED" || info?.onboarding;
          return (
            <div
              className={`${css.mainWrapper} ${
                onboarding ? css.mainWrapperWaves : css.mainWrapperStatic
              }`}
            >
              <PlaybackDataContextProvider>
                <div class="h-screen w-screen">
                  <Notifications />
                  {onboarding ? (
                    <Onboarding connectivity={info?.connectivity!} />
                  ) : (
                    <NormalComponent info={info} />
                  )}
                </div>
              </PlaybackDataContextProvider>
            </div>
          );
        }}
      </ReduxAPIFetcher>
    </>
  );
}
