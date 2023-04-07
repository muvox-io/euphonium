import { render } from "preact";
import { Provider } from "react-redux";
import { App } from "./components/App";
import store from "./redux/store";

render(
  <Provider store={store}>
    <App />
  </Provider>,
  document.getElementById("app")!
);
