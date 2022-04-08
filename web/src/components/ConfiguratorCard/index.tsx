import { useCallback, useEffect, useState } from "preact/hooks";
import {
  ConfigurationFieldType,
  PluginConfiguration,
} from "../../api/euphonium/plugins/models";
import PluginsAPI from "../../api/euphonium/plugins/PluginsAPI";
import useAPI from "../../utils/useAPI.hook";
import { useDebouncedCallback } from "../../utils/useDebouncedCallback";
import APIFetcher from "../APIFetcher";
import Dashboard from "../Dashboard";
import FormGroup from "../FormGroup";
import Button from "../ui/Button";
import Card from "../ui/Card";

function CardContents({
  pluginConfig,
  plugin,
  setPluginConfig,
}: {
  plugin: string;
  pluginConfig: PluginConfiguration;
  setPluginConfig: (pluginConfig: PluginConfiguration) => void;
}) {
  const pluginsAPI = useAPI(PluginsAPI);
  const [formValue, setFormValue] = useState(
    Object.fromEntries(
      pluginConfig.configSchema
        .filter((f) => f.type !== ConfigurationFieldType.GROUP)
        .map((field) => [field.key, field.value])
    )
  );

  const [shouldMakeRequest, setShouldMakeRequest] = useState(false);

  useEffect(() => {
    (async () => {
      if (shouldMakeRequest) {
        setShouldMakeRequest(false);
        const resp = await pluginsAPI.updatePluginConfiguration(
          plugin,
          formValue,
          true
        );
        setPluginConfig(resp);
      }
    })();
  }, [formValue, shouldMakeRequest]);

  const groupFields = pluginConfig.configSchema.filter(
    (field) => field.type === ConfigurationFieldType.GROUP
  );

  return (
    <Card title={pluginConfig.displayName} subtitle={"plugin configuration"}>
      {groupFields.map((group) => {
        const fieldsInGroup = pluginConfig.configSchema.filter(
          (field) => field.group == group.key
        );
        return (
          <FormGroup
            key={group.key}
            label={group.label}
            fields={fieldsInGroup}
            onChange={(value) => {
              setFormValue({ ...formValue, ...value });
            }}
            onChangeFinished={() => {
              setShouldMakeRequest(true);
            }}
            value={formValue}
          />
        );
      })}
      <div class="flex flex-col">
        <Button
          type="primary"
          class="self-stretch md:self-end"
          onClick={async () => {
            const resp = await pluginsAPI.updatePluginConfiguration(
              plugin,
              formValue,
              false // no preview
            );
            setPluginConfig(resp);
          }}
        >
          Apply changes
        </Button>
      </div>
    </Card>
  );
}

export default function ConfiguratorCard({ plugin }: { plugin: string }) {
  if (plugin == "home") {
    return <Dashboard />;
  }

  return (
    <APIFetcher
      api={PluginsAPI}
      fetch={(api) => api.getPluginConfiguration(plugin)}
      dependencies={[plugin]}
    >
      {(
        pluginConfig: PluginConfiguration,
        refresh: () => void,
        setPluginConfig: (p: PluginConfiguration) => void
      ) => {
        return (
          <CardContents
            plugin={plugin}
            pluginConfig={pluginConfig}
            setPluginConfig={setPluginConfig}
          />
        );
      }}
    </APIFetcher>
  );
}

// import { useContext, useEffect, useRef, useState } from "preact/hooks";
// import {
//   ConfigurationField,
//   ConfigurationFieldType,
//   PluginConfiguration,
// } from "../../api/euphonium/plugins/models";

// import { PlaybackDataContext } from "../../utils/PlaybackContext";
// import Dashboard from "../Dashboard";
// import Card from "../ui/Card";
// import Checkbox from "../ui/Checkbox";
// import IconCard from "../ui/IconCard";
// import Input from "../ui/Input";
// import NumberInput from "../ui/NumberInput";
// import Select from "../ui/Select";

// const renderConfigurationField = (
//   field: ConfigurationField,
//   updateField: (field: ConfigurationField, value: string) => void
// ) => {
//   let { type, label, value, values } = field;
//   const onChange = (e: string) => updateField(field, e);
//   switch (type) {
//     case ConfigurationFieldType.TEXT:
//       return <Input tooltip={label} value={value} onBlur={onChange} />;
//     case ConfigurationFieldType.NUMBER:
//       return <NumberInput tooltip={label} value={value} onBlur={onChange} />;
//     case ConfigurationFieldType.CHECKBOX:
//       return (
//         <Checkbox
//           value={value == "true"}
//           label={label!!}
//           onChange={(v) => {
//             updateField(field, v ? "true" : "false");
//           }}
//         />
//       );
//     case ConfigurationFieldType.SELECT:
//       return (
//         <Select
//           tooltip={label!!}
//           value={value}
//           values={values!!}
//           onChange={onChange}
//         />
//       );
//     default:
//       return <p>Unsupported field type: {field}</p>;
//   }
// };

// const ConfigurationGroup = ({
//   fields = [],
//   groupKey = "",
//   updateField,
// }: {
//   fields: ConfigurationField[];
//   groupKey: string;
//   updateField: (field: ConfigurationField, value: string) => void;
// }) => {
//   const group = fields.filter((e) => e.key == groupKey);
//   const groupFields = fields.filter((e) => e.group == groupKey);

//   return (
//     <IconCard iconName="settings" label={group[0].label}>
//       <div class="flex flex-col space-y-5 -mt-3">
//         {groupFields.map((field) =>
//           renderConfigurationField(field, updateField)
//         )}
//       </div>
//     </IconCard>
//   );
// };

// export default ({ plugin = "" }) => {
//   if (plugin == "home") {
//     return <Dashboard />;
//   }

//   const [displayName, setDisplayName] = useState<string>("");
//   const [configurationFields, setConfigurationFields] = useState<
//     ConfigurationField[]
//   >([]);

//   const [groups, setGroups] = useState<ConfigurationField[]>([]);

//   const [dirty, setDirty] = useState<boolean>(false);
//   const { setPlaybackState } = useContext(PlaybackDataContext);

//   const setConfig = ({ configSchema, displayName }: PluginConfiguration) => {
//     setGroups(
//       configSchema.filter((e) => e.type == ConfigurationFieldType.GROUP)
//     );
//     setIsLoading(false);
//     setConfigurationFields(configSchema);
//     setDisplayName(displayName);
//   };

//   const loadConfig = () => {
//     setIsLoading(true);
//     getPluginConfiguration(plugin).then((e) => {
//       setConfig(e);
//     });
//   };

//   useEffect(() => {
//     loadConfig();
//   }, [plugin]);

//   const updateConfiguration = async () => {
//     const config = await updatePluginConfiguration(
//       plugin,
//       fieldsToValues(configurationFields),
//       false
//     );
//     setConfig(config);
//     setDirty(false);
//   };

//   const updateField = async (field: ConfigurationField, value: string) => {
//     setDirty(true);
//     const newFields = configurationFields.map((f) => {
//       if (f === field) {
//         return { ...f, value };
//       }
//       return f;
//     });
//     const config = await updatePluginConfiguration(
//       plugin,
//       fieldsToValues(newFields),
//       true
//     );

//     setConfig(config);
//   };

//   if (isLoading) return null;
//   const btnRef = useRef<HTMLDivElement>(null);

//   return (
//     // <div class="flex flex-col m-10">
//     //   <div class="flex flex-row items-center">
//     //     <div class="flex flex-col">
//     //       <div class='text-3xl'>{displayName}</div>
//     //       <div class='text-m mt-2 text-app-text-secondary'>plugin configuration</div>
//     //     </div>
//     //     <div class="ml-auto flex-col text-red-400 animate-pulse mr-4">
//     //       {dirty ? 'Unsaved changes' : ''}
//     //     </div>
//     <Card title={displayName} subtitle={"plugin configuration"}>
//       <div class="grid grid-cols-3 gap-4 items-start">
//         {groups.length
//           ? groups.map((field) => (
//               <ConfigurationGroup
//                 updateField={updateField}
//                 fields={configurationFields}
//                 groupKey={field.key}
//               />
//             ))
//           : null}
//       </div>
//     </Card>
//     // <Card title={displayName} subtitle={"plugin configuration"}>
//     //   <div class="flex flex-col items-start space-y-2 md:max-w-[400px]">
//     //     {plugin == "dac" ? (
//     //       <DACConfig
//     //         configurationUpdated={() => {
//     //           loadConfig();
//     //         }}
//     //       ></DACConfig>
//     //     ) : null}
//     //     {plugin == "ota" ? <OTATrigger /> : null}

//     //     {groups.length ? groups.map((field) => (<ConfigurationGroup
//     //       updateField={updateField}
//     //       fields={configurationFields}
//     //       groupKey={field.key} />)) : null}

//     //     <Button disabled={!dirty} onClick={updateConfiguration}>
//     //       Apply changes
//     //     </Button>
//     //   </div>
//     // </Card>
//   );
// };
