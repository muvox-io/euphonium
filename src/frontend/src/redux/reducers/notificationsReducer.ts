import { createSlice } from "@reduxjs/toolkit";
import { NotificationMessagePayload } from "../api/websocket/websocketApi";
import { websocketMessage } from "./websocketReducer";

export interface NotificationInReducer extends NotificationMessagePayload {
  id: string;
  timestamp: number;
  expiryTimeout?: number;
}

export interface NotificationsReducerState {
  notifications: NotificationInReducer[];
}

const notificationsSlice = createSlice({
  name: "notifications",
  initialState: {
    notifications: [],
  } as NotificationsReducerState,
  reducers: {
    expireNotification: (
      state,
      action: {
        payload: string;
      }
    ) => {
      state.notifications = state.notifications.filter(
        (n) => n.id !== action.payload
      );
    },
  },
  extraReducers: (builder) => {
    builder.addCase(websocketMessage, (state, action) => {
    
      if (action.payload.type === "notification") {
      
        state.notifications.push({
          ...action.payload.data,
          id: action.meta.id,
          timestamp: action.meta.timestamp,
          expiryTimeout: 7500,
        });
      }
    });
  },
});

export const notificationsReducer = notificationsSlice.reducer;

export const { expireNotification } = notificationsSlice.actions;
