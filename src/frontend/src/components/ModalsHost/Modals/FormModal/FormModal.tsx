import React from "react";
import { ConfigurationFieldType } from "../../../../api/euphonium/plugins/models";
import { useGetPluginConfigurationQuery } from "../../../../redux/api/euphonium/pluginsApi";
import { ModalConfig } from "../../../../redux/reducers/modalsReducer";
import FormGroup from "../../../FormGroup";
import ReduxAPIFetcher from "../../../ReduxAPIFetcher";
import Modal from "../../../ui/Modal";

const FormModal = ({
  config,
}: {
  config: ModalConfig<{ pluginName: string; modalGroupKey: string }>;
}) => {
  const result = useGetPluginConfigurationQuery(config.data.pluginName);
  return (
    <Modal header={config.title}>
      <ReduxAPIFetcher result={result}>
        {({ data }) => {
          const fields = data!.configSchema.filter(
            (f) => f.group === config.data.modalGroupKey
          );

          const value = Object.fromEntries(
            fields
              .filter(
                (f) =>
                  f.type !== ConfigurationFieldType.GROUP &&
                  f.type !== ConfigurationFieldType.MODAL_GROUP
              )
              .map((field) => [field.key, field.value])
          );

          return (
            <FormGroup
              fields={fields}
              value={value}
              onChange={() => {}}
              onChangeFinished={() => {}}
            />
          );
        }}
      </ReduxAPIFetcher>
    </Modal>
  );
};

export default FormModal;
