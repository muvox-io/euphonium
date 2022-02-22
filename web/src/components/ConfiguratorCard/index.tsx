import { useState, useEffect, useContext } from "preact/hooks";

import Input from "../Input";
import Select from "../Select";
import Modal from "../Modal";

import Card from "../Card";
import Button from "../Button";
import DACConfig from "../../apps/DACConfig";
import OTATrigger from "../../apps/ota/OTATrigger";
import { PlaybackDataContext } from "../../utils/PlaybackContext";
import { getPluginConfiguration, updatePluginConfiguration, fieldsToValues, ConfigurationFieldType, ConfigurationField, PluginConfiguration } from "../../api/euphonium/plugins";
import Checkbox from "../Checkbox";

const renderConfigurationField = (
  field: ConfigurationField,
  updateField: (field: ConfigurationField, value: string) => void
) => {
  let { type, label, value, values } = field;
  const onChange = (e: string) => updateField(field, e);
  switch (type) {
    case ConfigurationFieldType.TEXT:
      return <Input tooltip={label} value={value} onBlur={onChange} />;
    case ConfigurationFieldType.NUMBER:
      return <Input tooltip={label} value={value} onBlur={onChange} type={'number'} />;
    case ConfigurationFieldType.CHECKBOX:
      return (<Checkbox value={value == "true"} label={label!!} onChange={(v) => {
        updateField(field, v ? "true" : "false");
      }} />);
    case ConfigurationFieldType.SELECT:
      return (
        <Select
          tooltip={label!!}
          value={value}
          values={values!!}
          onChange={onChange}
        />
      );
    default:
      return <p>Unsupported field type</p>;
  }
};

const ConfigurationGroup = ({ fields = [], groupKey = "", updateField }: { fields: ConfigurationField[], groupKey: string, updateField: (field: ConfigurationField, value: string) => void }) => {
  const group = fields.filter((e) => e.key == groupKey);
  const groupFields = fields.filter((e) => e.group == groupKey);

  return (<div class="flex flex-col w-full">
    <span class="text-lg font-thin">{group[0].label}</span>
    <div class="space-y-2 w-full">
      {groupFields.map((field) => renderConfigurationField(field, updateField))}
    </div>
    <div class="w-full h-[0.5px] opacity-70 mt-6 bg-app-text-secondary mb-2"></div>
  </div>);
}

export default ({ plugin = "" }) => {
  const [isLoading, setIsLoading] = useState<boolean>(true);
  const [displayName, setDisplayName] = useState<string>("");
  const [configurationFields, setConfigurationFields] = useState<
    ConfigurationField[]
  >([]);

  const [groups, setGroups] = useState<
    ConfigurationField[]
  >([]);

  const [dirty, setDirty] = useState<boolean>(false);
  const { setPlaybackState } = useContext(PlaybackDataContext);

  const setConfig = ({ configSchema, displayName }: PluginConfiguration) => {
    setGroups(configSchema.filter((e) => e.type == ConfigurationFieldType.GROUP));
    setIsLoading(false);
    setConfigurationFields(configSchema);
    setDisplayName(displayName);
  }

  const loadConfig = () => {
    setIsLoading(true);
    getPluginConfiguration(plugin).then((e) => {
      setConfig(e);
    });
  };

  useEffect(() => {
    loadConfig();
  }, [plugin]);

  const updateConfiguration = async () => {
    const config = await updatePluginConfiguration(plugin, fieldsToValues(configurationFields), false);
    setConfig(config);
    setDirty(false);
  };

  const updateField = async (field: ConfigurationField, value: string) => {
    setDirty(true);
    const newFields = configurationFields.map((f) => {
      if (f === field) {
        return { ...f, value };
      }
      return f;
    });
    const config = await updatePluginConfiguration(plugin, fieldsToValues(newFields), true);

    setConfig(config);
  };

  if (isLoading) return null;

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

        {groups.length ? groups.map((field) => (<ConfigurationGroup
          updateField={updateField}
          fields={configurationFields}
          groupKey={field.key} />)) : null}

        <Button disabled={!dirty} onClick={updateConfiguration}>
          Apply changes
        </Button>
      </div>
    </Card>
  );
};
