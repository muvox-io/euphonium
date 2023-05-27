import { JSX } from "preact";
import { useEffect, useState } from "preact/hooks";
import { useSelector } from "react-redux";
import getBaseUrl from "../../../../api/euphonium/baseUrl";
import { WebsocketStatus } from "../../../../redux/reducers/websocketReducer";
import Modal from "../../../ui/Modal";
import Spinner from "../../../ui/Spinner";

/**
 * ConnectionLostModal detects when the eventSource used for notifications fails and displays a modal
 * prompting the user to check his connection.
 */
export default function ConnectionLostModal(): JSX.Element | null {
  let websocketStatus = useSelector(
    (state: any) => state.websocket.status as WebsocketStatus
  );
  // const open =
  //   websocketStatus !== WebsocketStatus.CONNECTED &&
  //   websocketStatus !== WebsocketStatus.INIIAL;
  const isConnecting = websocketStatus === WebsocketStatus.CONNECTING;
  let [showImpulse, setShowImpulse] = useState(false); // determines whether the modal should show the impulse animation, which is shown on every error
  useEffect(() => {
    if (isConnecting) {
      setShowImpulse(true);
      const timeout = setTimeout(() => {
        setShowImpulse(false);
      }, 501);
      return () => clearTimeout(timeout);
    }
    return () => {};
  }, [isConnecting]);
  return (
    <Modal header="Connection lost">
      <div class="flex flex-col mt-16">
        <Spinner
          class="inline-block self-center"
          spinnerSize="large"
          showImpulse={showImpulse}
        ></Spinner>
        <div class="self-center inline-block mt-4">Reconnecting...</div>
        <div class="self-center inline-block mt-4 text-app-text-secondary text-lg">
          {getBaseUrl()}
        </div>
      </div>
      <p class="mt-16 text-app-text-secondary font-light text-lg">
        If the problem persists:
        <ul class="list-disc ml-8 mt-4">
          <li>Make sure your Euphonium device is turned on</li>
          <li>
            Check if your device is connected to the same network as this
            browser
          </li>
          <li>
            Ensure that both your Euphonium device and this browser have a
            strong Wi-Fi signal
          </li>
        </ul>
      </p>
    </Modal>
  );
}
