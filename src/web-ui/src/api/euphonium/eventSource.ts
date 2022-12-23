import getBaseUrl from "./baseUrl";


class EuphEventsEmitter {
  webSocket = new WebSocket(`${getBaseUrl().replace("http://", "ws://")}/events`);
  _events: Record<string, Array<(data: any) => void>> = {};

  constructor() {
    this.webSocket.onmessage = (event: any) => {
      const { type, data } = JSON.parse(event.data);
      this.emit(type, data);
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
      throw new Error(`Can't remove a listener. Event "${name}" doesn't exits.`);
    }

    const filterListeners = (listener: any) => listener !== listenerToRemove;

    this._events[name] = this._events[name].filter(filterListeners);
  }

  emit(name: string, data: string) {
    if (!this._events[name]) {
      throw new Error(`Can't emit an event. Event "${name}" doesn't exits.`);
    }

    const fireCallbacks = (callback: any) => {
      callback(data);
    };

    this._events[name].forEach(fireCallbacks);
  }
}

export default new EuphEventsEmitter();