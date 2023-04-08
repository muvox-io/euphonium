import {
  ConfigurationFieldType,
  ConfigurationModalGroup,
} from "../../api/euphonium/plugins/models";
import {
  useGetGlobalModalsQuery,
  useGetPluginConfigurationQuery,
} from "../../redux/api/euphonium/pluginsApi";
import { ModalType } from "../../redux/reducers/modalsReducer";
import ModalTrigger from "../ModalTrigger";

const GlobalModalsForPlugin = ({ pluginName }: { pluginName: string }) => {
  const { data, isSuccess } = useGetPluginConfigurationQuery(pluginName);
  if (!isSuccess || !data) return <div></div>;
  return (
    <>
      {data.configSchema
        .filter((f) => f.type === ConfigurationFieldType.MODAL_GROUP)
        .map((f: ConfigurationModalGroup) => {
          return (
            <ModalTrigger
              key={pluginName + f.id}
              id={pluginName + "." + f.id}
              type={ModalType.FORM_MODAL}
              priority={f.priority}
              dismissable={f.dismissable}
              title={f.title}
              data={{
                pluginName,
                fields: f.children
              }}
            />
          );
        })}
    </>
  );
};

/**
 * Fetches the plugins which want to show a global modal,
 * and renders them.
 * @returns
 */
const GlobalModals = () => {
  const { data, isSuccess } = useGetGlobalModalsQuery();
  if (!isSuccess) return <div></div>;
  return (
    <>
      {data?.map((pluginName) => (
        <GlobalModalsForPlugin pluginName={pluginName} key={pluginName} />
      ))}
    </>
  );
};

export default GlobalModals;
