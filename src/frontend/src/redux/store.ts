import { configureStore } from "@reduxjs/toolkit";
import { setupListeners } from "@reduxjs/toolkit/dist/query";
import { EuphoniumApi } from "./api/euphonium/euphoniumApi";
import { websocketReducer } from "./reducers/websocketReducer";

const store = configureStore({
  reducer: {
    [EuphoniumApi.reducerPath]: EuphoniumApi.reducer,
    websocket: websocketReducer,
  },
  middleware: (getDefaultMiddleware) =>
    getDefaultMiddleware().concat(EuphoniumApi.middleware),
});

export default store;

setupListeners(store.dispatch);
