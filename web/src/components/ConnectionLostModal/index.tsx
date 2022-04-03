import { useContext, useEffect, useState } from "preact/hooks";
import { APIAccessorContext } from "../../api/APIAccessor";
import {} from "../../api/euphonium/api";
import getBaseUrl from "../../api/euphonium/baseUrl";
import eventSource from "../../api/euphonium/eventSource";
import Modal from "../Modal";
import Spinner from "../ui/Spinner";

/**
 * ConnectionLostModal dects wjen the eventSource used for notifications fails and displays a modal
 * prompting the user to check his connection.
 */
export default function ConnectionLostModal() {
  let [open, setOpen] = useState(false); // determines whether the modal should be shown
  let [showImpulse, setShowImpulse] = useState(false); // determines whether the modal should show the impulse animation, which is shown on every error
  let apiAccessor = useContext(APIAccessorContext);
  useEffect(() => {
    let errorListener = (e: any) => {
      console.log(e);
      setShowImpulse(true);
      setOpen(true);
      setTimeout(() => {
        setShowImpulse(false);
      }, 501);
    };
    let openListener = (e: any) => {
      setOpen(false);
      apiAccessor.notifyReconnect();
    };
    eventSource.addEventListener("error", errorListener);
    eventSource.addEventListener("open", openListener);
    return () => {
      eventSource.removeEventListener("error", errorListener);
      eventSource.removeEventListener("open", openListener);
    };
  }, [eventSource, setShowImpulse, setOpen]);
  return (
    open && (
      <Modal header="Connection lost">
        <div class="flex flex-col mt-16">
          <Spinner
            class="inline-block self-center"
            spinnerSize="large"
            showImpulse={showImpulse}
          ></Spinner>
          <div class="self-center inline-block mt-4">Reconnecting...</div>
          <div class="self-center inline-block mt-4 text-app-text-secondary">
            {getBaseUrl()}
          </div>
        </div>
        <p class="mt-16 text-app-text-secondary">
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
    )
  );
}
