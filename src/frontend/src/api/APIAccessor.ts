import { createContext } from "preact";
import getBaseUrl from "./euphonium/baseUrl";

export default class APIAccessor extends EventTarget {
  /**
   * Caches instances of API classes.
   */
  apiCache: { [key: string]: any } = {};

  responseCache: { [key: string]: any } = {};

  constructor(private baseURL: string) {
    super();
  }
  async fetch<R>(method: string, url: string, body?: any): Promise<R> {
    let headers: any = {
      Accept: "application/json",
    };
    if (body) {
      headers["Content-Type"] = "application/json";
    }
    const response = await fetch(this.baseURL + url, {
      method: method,
      body: body ? JSON.stringify(body) : undefined,
      headers,
    });
    if (!response.ok) {
      throw new Error(
        `${response.status} ${response.statusText}: ${await response.text()}`
      );
    }

    try {
      return await response.json();
    } catch(e) {
      throw new Error(`Failed to parse JSON from ${method} ${url}: ${e}`);
    }
  }

  notifyReconnect() {
    this.responseCache = {};
    this.dispatchEvent(new Event("reconnect"));
  }
}

export const APIAccessorContext = createContext<APIAccessor>(
  new APIAccessor(getBaseUrl())
);
