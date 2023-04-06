import React from "react";
import { useSelector } from "react-redux";
import { ModalType } from "../../redux/reducers/modalsReducer";
import { WebsocketStatus } from "../../redux/reducers/websocketReducer";
import ModalTrigger from "../ModalTrigger";

const ConnectionLostModalTrigger = () => {
  let websocketStatus = useSelector(
    (state: any) => state.websocket.status as WebsocketStatus
  );
  const open =
    websocketStatus !== WebsocketStatus.CONNECTED &&
    websocketStatus !== WebsocketStatus.INIIAL;
  return open ? (
    <ModalTrigger
      type={ModalType.CONNECTION_LOST}
      id="CONN_LOST"
      priority={10000}
    />
  ) : null;
};

export default ConnectionLostModalTrigger;
