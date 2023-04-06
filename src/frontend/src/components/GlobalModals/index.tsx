import { useGetGlobalModalsQuery } from "../../redux/api/euphonium/pluginsApi";

/**
 * Fetches the plugins which want to show a global modal,
 * and renders them.
 * @returns
 */
const GlobalModals = () => {
  const { data, isSuccess } = useGetGlobalModalsQuery();
  if (!isSuccess) return <div></div>;
  return <div>{JSON.stringify(data)}</div>;
};

export default GlobalModals;
