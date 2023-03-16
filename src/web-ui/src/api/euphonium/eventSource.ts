import getBaseUrl from "./baseUrl";

class EuphEventsEmitter {
  webSocket?: WebSocket;
  _events: Record<string, Array<(data: any) => void>> = {};
  isConnected = false;

  constructor() {
    this.connect();
  }

  connect() {
    this.webSocket = new WebSocket(
      `${getBaseUrl().replace("http://", "ws://")}/events`
    );
    this.webSocket.onmessage = (event: any) => {
      const { type, data } = JSON.parse(event.data);
      this.emit(type, data);
    };
    this.webSocket.onclose = () => {
      this.emit("error", null);
      this.isConnected = false;
    };
    this.webSocket.onerror = () => {
      this.emit("error", null);
      this.isConnected = false;
      setTimeout(() => {
        this.connect();
      }, 2000);
    };
    this.webSocket.onopen = () => {
      this.emit("open", null);
      this.isConnected = true;
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

export default new EuphEventsEmitter();
