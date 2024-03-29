import { configureStore } from "@reduxjs/toolkit";
import { setupListeners } from "@reduxjs/toolkit/dist/query";
import { EuphoniumApi } from "./api/euphonium/euphoniumApi";
import { modalsReducer } from "./reducers/modalsReducer";
import { pluginConfigurationsReducer } from "./reducers/pluginConfigurationsReducer";
import { websocketReducer } from "./reducers/websocketReducer";
import { notificationsReducer } from "./reducers/notificationsReducer";
import { localPlaybackStateReducer } from "./reducers/localPlaybackState";
import { radiobrowserReducer } from "./reducers/radiobrowserReducer";

const store = configureStore({
  devTools: process.env.NODE_ENV !== 'production',
  reducer: {
    [EuphoniumApi.reducerPath]: EuphoniumApi.reducer,
    websocket: websocketReducer,
    modals: modalsReducer,
    pluginConfigurations: pluginConfigurationsReducer,
    notifications: notificationsReducer,
    radiobrowser: radiobrowserReducer,
    localPlaybackState: localPlaybackStateReducer,
  },
  middleware: (getDefaultMiddleware) =>
    getDefaultMiddleware().concat(EuphoniumApi.middleware),
});

export default store;

export type RootState = ReturnType<typeof store.getState>;

setupListeners(store.dispatch);
