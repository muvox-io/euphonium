import { useState, useEffect, useContext, useRef } from "preact/hooks";

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
import Icon from "../Icon";
import NumberInput from "../NumberInput";
import Dashboard from "../Dashboard";

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
      return <NumberInput tooltip={label} value={value} onBlur={onChange} />;
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

  return (<div class="w-full bg-app-primary mt-8 md:p-4 rounded-2xl flex flex-col">
    <div class="flex flex-row text-bold items-center text-l text-app-accent">
      <span class="text-2xl">
        <Icon name="settings"></Icon>
      </span>
      {group[0].label}
    </div>
    <div class="flex flex-col pl-2 pr-2 space-y-5 mt-2 mb-2">
      {groupFields.map((field) => renderConfigurationField(field, updateField))}
    </div>
  </div>);
}

export default ({ plugin = "" }) => {
  if (plugin == "home") {
    return <Dashboard />;
  }


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
  const btnRef = useRef<HTMLDivElement>(null);

  return (
    // <div class="flex flex-col m-10">
    //   <div class="flex flex-row items-center">
    //     <div class="flex flex-col">
    //       <div class='text-3xl'>{displayName}</div>
    //       <div class='text-m mt-2 text-app-text-secondary'>plugin configuration</div>
    //     </div>
    //     <div class="ml-auto flex-col text-red-400 animate-pulse mr-4">
    //       {dirty ? 'Unsaved changes' : ''}
    //     </div>
    <Card title={displayName} subtitle={"plugin configuration"}>

      <div class="grid grid-cols-3 gap-4">
        {groups.length ? groups.map((field) => (<ConfigurationGroup
          updateField={updateField}
          fields={configurationFields}
          groupKey={field.key} />)) : null}
      </div>
    </Card>
    // <Card title={displayName} subtitle={"plugin configuration"}>
    //   <div class="flex flex-col items-start space-y-2 md:max-w-[400px]">
    //     {plugin == "dac" ? (
    //       <DACConfig
    //         configurationUpdated={() => {
    //           loadConfig();
    //         }}
    //       ></DACConfig>
    //     ) : null}
    //     {plugin == "ota" ? <OTATrigger /> : null}

    //     {groups.length ? groups.map((field) => (<ConfigurationGroup
    //       updateField={updateField}
    //       fields={configurationFields}
    //       groupKey={field.key} />)) : null}

    //     <Button disabled={!dirty} onClick={updateConfiguration}>
    //       Apply changes
    //     </Button>
    //   </div>
    // </Card>
  );
};
