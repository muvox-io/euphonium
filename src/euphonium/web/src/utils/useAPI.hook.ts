import { useContext } from "preact/hooks";
import APIAccessor, { APIAccessorContext } from "../api/APIAccessor";

export default function useAPI<A>(api: new (acc: APIAccessor) => A): A {
  const accessor = useContext(APIAccessorContext);
  if (accessor.apiCache[api.name]) {
    return accessor.apiCache[api.name];
  }
  console.log( accessor.apiCache)
  const apiInstance = new api(accessor);
  accessor.apiCache[api.name] = apiInstance;
  return apiInstance;
}
