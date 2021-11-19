import { useState, useEffect } from "preact/hooks";

import css from "./ConfiguratorCard.module.scss";
import Input from "../Input";
import Select from "../Select";
import Button from "../Button";
import { ConfigurationField, ConfigurationFieldType } from "../../api/euphonium/models";
import { getPluginConfiguration, updatePluginConfiguration } from "../../api/euphonium/api";
import RadioBrowser from "../../apps/radiobrowser/RadioBrowser";

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
    console.log(newFields);
  };

  return (
    <div className={css.confWrapper}>
      <div className={css.confWrapper__header}>{displayName} ({plugin})</div>
      <div className={css.confWrapper__subheader}>plugin configuration</div>
      <div className={css.confWrapper__card}>
        {/* {configurationFields.map((field) =>
          renderConfigurationField(field, updateField)
        )} */}
        <RadioBrowser></RadioBrowser>
      </div>
      <div className={css.confWrapper__actionButton}>
        {/* <Button onClick={() => { updateConfiguration() }} /> */}
      </div>
    </div>
  );
};
