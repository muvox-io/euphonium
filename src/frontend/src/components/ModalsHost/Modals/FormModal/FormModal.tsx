import React from "react";
import { ModalConfig } from "../../../../redux/reducers/modalsReducer";
import Modal from "../../../ui/Modal";

const FormModal = ({ config }: { config: ModalConfig }) => {
  return <Modal header={config.title}>ddddd</Modal>;
};

export default FormModal;
