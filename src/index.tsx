import { render } from "preact";
import "preact/hooks";
import App from "./App"
import "milligram/dist/milligram.min.css";

const appRoot = document.createElement("div");
appRoot.id = "app";
document.body.appendChild(appRoot);

render(
  // @ts-ignore
  <App/>,
  appRoot
);