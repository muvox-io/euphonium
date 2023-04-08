import {
  PluginConfiguration
} from "../../api/euphonium/plugins/models";
import { useGetPluginConfigurationQuery } from "../../redux/api/euphonium/pluginsApi";
import { pluginDirtySelector } from "../../redux/reducers/pluginConfigurationsReducer";
import Dashboard from "../Dashboard";
import FormFields from "../FormFields";
import ReduxAPIFetcher from "../ReduxAPIFetcher";
import Button from "../ui/Button";
import Card from "../ui/Card";

function CardContents({
  pluginConfig,
  pluginName,
}: {
  pluginName: string;
  pluginConfig: PluginConfiguration;
}) {
  const isDirty = pluginDirtySelector(pluginName);
  return (
    <Card title={pluginConfig.displayName} subtitle={"plugin configuration"}>
      <FormFields fields={pluginConfig.configSchema} pluginName={pluginName} />
      <div class="flex flex-col mt-[50px] md:mt-0">
        <Button
          type="primary"
          disabled={!isDirty}
          class="self-stretch md:self-end"
          onClick={async () => {}}
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

  const result = useGetPluginConfigurationQuery(plugin);

  return (
    <ReduxAPIFetcher result={result}>
      {({ data }) => {
        return <CardContents pluginName={plugin} pluginConfig={data!} />;
      }}
    </ReduxAPIFetcher>
  );
}
