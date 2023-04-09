import { useDispatch } from "react-redux";
import { ConfigurationField } from "../../../../api/euphonium/plugins/models";
import { ModalConfig } from "../../../../redux/reducers/modalsReducer";
import { onFieldEvent } from "../../../../redux/reducers/pluginConfigurationsReducer";
import FormFields from "../../../FormFields";
import Modal from "../../../ui/Modal";

const FormModal = ({
  config,
}: {
  config: ModalConfig<{
    pluginName: string;
    fields: ConfigurationField[];
    fieldId: string;
  }>;
}) => {
  const dispatch = useDispatch();
  return (
    <Modal
      header={config.title}
      onBackdropClick={async () => {
        try {
          dispatch(
            onFieldEvent({
              pluginName: config.data.pluginName,
              event: {
                fieldId: config.data.fieldId,
                name: "dismiss",
              },
            }) as any
          );
        } catch (e) {}
      }}
    >
      <FormFields
        fields={config.data.fields}
        pluginName={config.data.pluginName}
      />
    </Modal>
  );
};

export default FormModal;
