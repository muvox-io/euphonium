import { configureStore } from "@reduxjs/toolkit";
import { setupListeners } from "@reduxjs/toolkit/dist/query";
import { EuphoniumApi } from "./api/euphonium/euphoniumApi";
import { modalsReducer } from "./reducers/modalsReducer";
import { websocketReducer } from "./reducers/websocketReducer";

const store = configureStore({
  reducer: {
    [EuphoniumApi.reducerPath]: EuphoniumApi.reducer,
    websocket: websocketReducer,
    modals: modalsReducer,
  },
  middleware: (getDefaultMiddleware) =>
    getDefaultMiddleware().concat(EuphoniumApi.middleware),
});

export default store;

export type RootState = ReturnType<typeof store.getState>;

setupListeners(store.dispatch);