import { createAction, createReducer, nanoid } from "@reduxjs/toolkit";

export enum WebsocketStatus {
  INIIAL = "INITIAL",
  CONNECTING = "CONNECTING",
  WAITING_FOR_RECONNECT = "WAITING_FOR_RECONNECT",
  CONNECTED = "CONNECTED",
}

export interface WebsocketState {
  status: WebsocketStatus;
  ping: number | null;
}

const initialState: WebsocketState = {
  status: WebsocketStatus.INIIAL,
  ping: null,
};

export const statusChanged = createAction<WebsocketStatus>(
  "websocket/statusChanged"
);

export const pingReceived = createAction<number>("websocket/pingReceived");

export const websocketMessage = createAction(
  "websocket/message",
  (type: any, data: any) => {
    if (typeof type !== "string") {
      throw new Error("Websocket message type must be a string");
    }
    return {
      type: "websocket/message",
      payload: { type, data },
      meta: {
        timestamp: new Date().getTime(),
        id: nanoid(),
      },
    };
  }
);

export const websocketReducer = createReducer(initialState, (builder) => {
  builder
    .addCase(statusChanged, (state, action) => {
      state.status = action.payload;
    })
    .addCase(pingReceived, (state, action) => {
      state.ping = action.payload;
    });
});
