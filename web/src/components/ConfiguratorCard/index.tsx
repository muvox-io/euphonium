import { useState, useEffect } from "preact/hooks";

import Input from "../Input";
import Select from "../Select";
import { ConfigurationField, ConfigurationFieldType } from "../../api/euphonium/models";
import { getPluginConfiguration, updatePluginConfiguration } from "../../api/euphonium/api";
import Card from "../Card";

const renderConfigurationField = (
  field: ConfigurationField,
  updateField: (field: ConfigurationField, value: string) => void
) => {
  let { type, tooltip, value, listValues } = field;
  const onChange = (e: string) => updateField(field, e);
  switch (type) {
    case ConfigurationFieldType.String:
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
    default:
      return <p>Unsupported field type</p>;
  }
};

export default ({ plugin = "" }) => {
  const [displayName, setDisplayName] = useState<string>("");
  const [configurationFields, setConfigurationFields] = useState<
    ConfigurationField[]
  >([]);

  useEffect(() => {
    getPluginConfiguration(plugin).then((e) => {
      setConfigurationFields(e.fields);
      setDisplayName(e.displayName);
    });
  }, [plugin]);

  const updateConfiguration = () => {
    updatePluginConfiguration(plugin, configurationFields).then((e) => {
      setConfigurationFields(e.fields);
      setDisplayName(e.displayName);
    });
  };

  const updateField = (field: ConfigurationField, value: string) => {
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
      <div class='flex flex-col items-start space-y-2'>
        {configurationFields.map((field) =>
          renderConfigurationField(field, updateField)
        )}
      </div>
    </Card>
  );
};
