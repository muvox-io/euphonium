import { Component } from "preact";
import { useContext, useEffect, useState } from "preact/hooks";

import APIAccessor, { APIAccessorContext } from "../../api/APIAccessor";
import useAPI from "../../utils/useAPI.hook";

export interface APIFetcherProps<A, R> {
  api: new (acc: APIAccessor) => A;
  fetch: (api: A) => Promise<R>;
  errorComponent?: Component<{ error: string }, {}>;
  loadingComponent?: Component;
  children: (resp: R) => JSX.Element;
  dependencies?: any[];
  cacheKey?: string;
}

enum State {
  Loading,
  Loaded,
  Error,
}

export default function APIFetcher<A, R>({
  api,
  children,
  fetch: fetchFunc,
  dependencies = [],
  cacheKey,
}: APIFetcherProps<A, R>) {
  const apiAccessor = useContext(APIAccessorContext);
  const apiInstance = useAPI(api);
  const [state, setState] = useState(State.Loading);
  const [error, setError] = useState<string | undefined>();
  const [response, setResponse] = useState<R | undefined>();

  async function fetch() {
    let cacheKeyWithDependencies = cacheKey + dependencies.join("_");
    if (cacheKey && apiAccessor.responseCache[cacheKeyWithDependencies]) {
      setState(State.Loaded);
      setResponse(apiAccessor.responseCache[cacheKeyWithDependencies]);
      return;
    }

    try {
      setState(State.Loading);
      const response = await fetchFunc(apiInstance);
      setState(State.Loaded);
      if (cacheKey) {
        apiAccessor.responseCache[cacheKeyWithDependencies] = response;
      }
      setResponse(response);
    } catch (e) {
      setState(State.Error);
      setError((e as any).message);
    }
  }
  useEffect(() => {
    const reconnectListener = () => {
      fetch();
    };
    apiAccessor.addEventListener("reconnect", reconnectListener);
    return () => {
      apiAccessor.removeEventListener("reconnect", reconnectListener);
    };
  }, [apiInstance]);

  useEffect(() => {
    fetch();
  }, [...dependencies]);

  if (state === State.Loading) {
    return <div>Loading...</div>;
  }
  if (state === State.Error) {
    return <div>Error: {error}</div>;
  }

  return <div>{children(response as R)}</div>;
}
