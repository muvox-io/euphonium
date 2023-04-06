import {
  statusChanged,
  websocketMessage,
  WebsocketStatus,
} from "../../redux/reducers/websocketReducer";
import store from "../../redux/store";
import getBaseUrl from "./baseUrl";

class EuphEventsEmitter {
  webSocket!: WebSocket;
  _events: Record<string, Array<(data: any) => void>> = {};

  constructor(private reduxStore: typeof store) {
    this.connect();
  }

  connect() {
    this.reduxStore.dispatch(statusChanged(WebsocketStatus.CONNECTING));

    this.emit("connecting", null);
    this.webSocket = new WebSocket(
      `${getBaseUrl().replace("http://", "ws://")}/events`
    );
    this.webSocket.onmessage = (event: any) => {
      const msg = JSON.parse(event.data);
      this.reduxStore.dispatch(websocketMessage(msg.type, msg.data));
      this.emit(msg.type, msg.data);
    };
    this.webSocket.onerror = (event: any) => {
      this.reduxStore.dispatch(
        statusChanged(WebsocketStatus.WAITING_FOR_RECONNECT)
      );
      this.emit("error", event);
    };
    this.webSocket.onopen = (event: any) => {
      this.reduxStore.dispatch(statusChanged(WebsocketStatus.CONNECTED));
      this.emit("open", event);
    };
    this.webSocket.onclose = (event: any) => {
      this.reduxStore.dispatch(
        statusChanged(WebsocketStatus.WAITING_FOR_RECONNECT)
      );
      this.emit("close", event);
      setTimeout(() => {
        this.connect();
      }, 5000);
    };
  }

  on(name: string, listener: (data: any) => void) {
    if (!this._events[name]) {
      this._events[name] = [];
    }

    this._events[name].push(listener);
  }

  removeListener(name: string, listenerToRemove: any) {
    if (!this._events[name]) {
      throw new Error(
        `Can't remove a listener. Event "${name}" doesn't exits.`
      );
    }

    const filterListeners = (listener: any) => listener !== listenerToRemove;

    this._events[name] = this._events[name].filter(filterListeners);
  }

  emit(name: string, data: any) {
    if (!this._events[name]) {
      return;
    }

    const fireCallbacks = (callback: any) => {
      callback(data);
    };

    this._events[name].forEach(fireCallbacks);
  }
}

const eventSource = new EuphEventsEmitter(store);

export default eventSource;
