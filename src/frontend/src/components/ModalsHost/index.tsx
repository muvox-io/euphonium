import { useSelector } from "react-redux";
import { ModalsState, ModalType } from "../../redux/reducers/modalsReducer";
import { RootState } from "../../redux/store";
import ConnectionLostModal from "./Modals/ConnectionLostModal";

const modalComponents = {
  [ModalType.CONNECTION_LOST]: ConnectionLostModal,
};

const ModalsHost = () => {
  const { modals } = useSelector<RootState>(
    (state) => state.modals
  ) as ModalsState;
  if (modals.length <= 0) return null;
  const modal = modals[modals.length - 1];
  const Cmp = modalComponents[modal.type];
  return <Cmp />;
};

export default ModalsHost;
