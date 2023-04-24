import Modal from "../../ui/Modal";
import Button from "../../ui/Button";
import { FieldProps } from "./FieldProps";
import { ConfigurationModalConfirmField } from "../../../api/euphonium/plugins/models";
import { useDispatch, useSelector } from "react-redux";
import { onFieldChanged, pluginStateSelector } from "../../../redux/reducers/pluginConfigurationsReducer";

export default function ModalConfirm({
  field,
  pluginName
  
}: FieldProps<ConfigurationModalConfirmField>) {
  const pluginState = useSelector(pluginStateSelector(pluginName));
  const dispatch = useDispatch();
  let value = pluginState[field.stateKey];
  let okValue = field.okValue || true;
  let cancelValue = field.cancelValue || false;
  if (value === okValue || value === cancelValue) {
    return null;
  }
  return (
    <Modal header={field.label}>
      <p>{field.hint}</p>
      <div className="flex flex-col mt-10">
        <Button
          onClick={() => {
            dispatch(onFieldChanged({
              pluginName,
              key: field.stateKey,
              value: okValue,
              debounce: false
            }) as any)
            
          }}
          kind="primary"
        >
          OK
        </Button>
        <Button
          onClick={() => {
            dispatch(onFieldChanged({
              pluginName,
              key: field.stateKey,
              value: okValue,
              debounce: false
            }) as any)
          }}
          kind="borderless"
        >
          Cancel
        </Button>
      </div>
    </Modal>
  );
}
