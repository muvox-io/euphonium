import React from "react";
import { ConfigurationFieldType } from "../../../../api/euphonium/plugins/models";
import { useGetPluginConfigurationQuery } from "../../../../redux/api/euphonium/pluginsApi";
import { ModalConfig } from "../../../../redux/reducers/modalsReducer";
import FormGroup from "../../../FormFields";
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

          const value = data?.state;

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
