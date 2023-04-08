import { ConfigurationField } from "../../../../api/euphonium/plugins/models";
import { ModalConfig } from "../../../../redux/reducers/modalsReducer";
import FormFields from "../../../FormFields";
import Modal from "../../../ui/Modal";

const FormModal = ({
  config,
}: {
  config: ModalConfig<{ pluginName: string; fields: ConfigurationField[] }>;
}) => {
  return (
    <Modal header={config.title}>
      <FormFields
        fields={config.data.fields}
        pluginName={config.data.pluginName}
      />
    </Modal>
  );
};

export default FormModal;
