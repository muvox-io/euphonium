import { useEffect } from "preact/hooks";
import { PluginConfiguration } from "../../api/euphonium/plugins/models";
import { useGetPluginConfigurationQuery } from "../../redux/api/euphonium/pluginsApi";
import {
  pluginDirtySelector,
  pluginEventProcessingSelector,
} from "../../redux/reducers/pluginConfigurationsReducer";
import Dashboard from "../Dashboard";
import FormFields from "../FormFields";
import ReduxAPIFetcher from "../ReduxAPIFetcher";
import Button from "../ui/Button";
import Card from "../ui/Card";
import { useSelector } from "react-redux";

function CardContents({
  pluginConfig,
  pluginName,
}: {
  pluginName: string;
  pluginConfig: PluginConfiguration;
}) {
  return (
    <Card title={pluginConfig.displayName} subtitle={"plugin configuration"}>
      <FormFields fields={pluginConfig.configSchema} pluginName={pluginName} />
    </Card>
  );
}

export default function ConfiguratorCard({ plugin }: { plugin: string }) {
  if (plugin == "home") {
    return <Dashboard />;
  }

  const isProcessingEvent = useSelector(pluginEventProcessingSelector(plugin));
  const result = useGetPluginConfigurationQuery(plugin);
  useEffect(() => {
    if (result.data && result.data.refresh_interval && !isProcessingEvent) {
      const int = setInterval(() => {
        result.refetch();
      }, result.data.refresh_interval);
      return () => clearInterval(int);
    }
    return () => {};
  }, [result.data, isProcessingEvent]);
  return (
    <ReduxAPIFetcher result={result}>
      {({ data }) => {
        return <CardContents pluginName={plugin} pluginConfig={data!} />;
      }}
    </ReduxAPIFetcher>
  );
}
