import { useGetSystemInfoQuery } from "../../redux/api/euphonium/system/systemApi";
import Spinner from "../ui/Spinner";

export interface UsedReduxProperties {
  isFetching: boolean;
  isLoading: boolean;
  isSuccess: boolean;
  isError: boolean;
  error?: any;
}

export interface ReduxAPIFetcherProps<T extends UsedReduxProperties> {
  result: T;
  children: (result: T) => JSX.Element;
}

export default function ReduxAPIFetcher<T extends UsedReduxProperties>(
  props: ReduxAPIFetcherProps<T>
) {
  if (props.result.isLoading) {
    return <Spinner></Spinner>;
  }
  if (props.result.isError) {
    return (
      <div class="p-5 m-2 rounded-md bg-app-accent box-border w-fit">
        Error: {props.result.error.error}
      </div>
    );
  }

  return props.children(props.result);
}
