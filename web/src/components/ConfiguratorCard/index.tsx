import { useState, useEffect } from "preact/hooks";

import Input from "../Input";
import Select from "../Select";
import Modal from "../Modal";
import {
  ConfigurationField,
  ConfigurationFieldType,
} from "../../api/euphonium/models";
import {
  getPluginConfiguration,
  updatePluginConfiguration,
} from "../../api/euphonium/api";
import Card from "../Card";
import Button from "../Button";
import DACConfig from "../../apps/DACConfig";
import OTATrigger from "../../apps/ota/OTATrigger";

const renderConfigurationField = (
  field: ConfigurationField,
  updateField: (field: ConfigurationField, value: string) => void
) => {
  let { type, tooltip, value, listValues } = field;
  const onChange = (e: string) => updateField(field, e);
  switch (type) {
    case ConfigurationFieldType.String:
      return <Input tooltip={tooltip} value={value} onChange={onChange} />;
    case ConfigurationFieldType.Number:
      return <Input tooltip={tooltip} value={value} onChange={onChange} />;
    case ConfigurationFieldType.StringList:
      return (
        <Select
          tooltip={tooltip}
          value={value}
          values={listValues}
          onChange={onChange}
        />
      );
    case ConfigurationFieldType.Hidden:
      return <></>;
    default:
      return <p>Unsupported field type</p>;
  }
};

export default ({ plugin = "" }) => {
  const [isLoading, setIsLoading] = useState<boolean>(true);
  const [displayName, setDisplayName] = useState<string>("");
  const [configurationFields, setConfigurationFields] = useState<
    ConfigurationField[]
  >([]);

  const [dirty, setDirty] = useState<boolean>(false);

  const loadConfig = () => {
    getPluginConfiguration(plugin).then((e) => {
      setConfigurationFields(e.fields);
      setDisplayName(e.displayName);
    });
  };

  useEffect(() => {
    loadConfig();
  }, [plugin]);

  const updateConfiguration = () => {
    updatePluginConfiguration(plugin, configurationFields).then((e) => {
      setConfigurationFields(e.fields);
      setDisplayName(e.displayName);
      setDirty(false);
    });
  };

  const updateField = (field: ConfigurationField, value: string) => {
    setDirty(true);
    const newFields = configurationFields.map((f) => {
      if (f === field) {
        return { ...f, value };
      }
      return f;
    });
    setConfigurationFields(newFields);
  };

  return (
    <Card title={displayName} subtitle={"plugin configuration"}>
      <div class="flex flex-col items-start space-y-2 md:max-w-[400px]">
        {plugin == "dac" ? (
          <DACConfig
            configurationUpdated={() => {
              loadConfig();
            }}
          ></DACConfig>
        ) : null}
        {plugin == "ota" ? <OTATrigger /> : null}

        {configurationFields.map((field) =>
          renderConfigurationField(field, updateField)
        )}
        <div class="pt-3">
          <Button disabled={!dirty} onClick={updateConfiguration}>
            Apply changes
          </Button>
        </div>
      </div>
    </Card>
  );
};
