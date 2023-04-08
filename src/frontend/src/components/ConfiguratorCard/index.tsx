import { useEffect, useState } from "preact/hooks";
import {
  ConfigurationFieldType,
  PluginConfiguration,
} from "../../api/euphonium/plugins/models";
import PluginsAPI from "../../api/euphonium/plugins/PluginsAPI";
import useAPI from "../../utils/useAPI.hook";
import APIFetcher from "../APIFetcher";
import Dashboard from "../Dashboard";
import FormFields from "../FormFields";
import FormGroup from "../FormFields";
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
  const [formValue, setFormValue] = useState(pluginConfig.configSchema);

  const [isDirty, setIsDirty] = useState(false);

  useEffect(() => {
    setFormValue({
      ...formValue,
      ...pluginConfig.state,
    });
  }, [pluginConfig]);

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

  return (
    <Card title={pluginConfig.displayName} subtitle={"plugin configuration"}>
      <FormFields fields={pluginConfig.configSchema} value={formValue}  />
      <div class="flex flex-col mt-[50px] md:mt-0">
        <Button
          type="primary"
          disabled={!isDirty}
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
