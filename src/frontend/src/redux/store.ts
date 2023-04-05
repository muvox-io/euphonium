import { configureStore } from "@reduxjs/toolkit";
import { setupListeners } from "@reduxjs/toolkit/dist/query";
import { SystemApi } from "./api/euphonium/system/systemApi";
import { websocketReducer } from "./reducers/websocketReducer";

const store = configureStore({
  reducer: {
    [SystemApi.reducerPath]: SystemApi.reducer,
    websocket: websocketReducer,
  },
  middleware: (getDefaultMiddleware) =>
    getDefaultMiddleware().concat(SystemApi.middleware),
});

export default store;

setupListeners(store.dispatch);
