import { useDispatch, useSelector } from "react-redux";
import { ConfigurationFieldText } from "../../../api/euphonium/plugins/models";
import {
  onFieldChanged,
  pluginStateSelector,
} from "../../../redux/reducers/pluginConfigurationsReducer";
import Input from "../../ui/Input";
import { FieldProps } from "./FieldProps";

export default function TextField({
  field,
  pluginName,
}: FieldProps<ConfigurationFieldText>) {
  const pluginState = useSelector(pluginStateSelector(pluginName));
  const dispatch = useDispatch();
  const val =
    pluginState[field.stateKey] == null
      ? field.default
      : pluginState[field.stateKey];
  return (
    <Input
      tooltip={field.label}
      value={val}
      onChange={(str) =>
        dispatch(
          onFieldChanged({
            pluginName,
            key: field.stateKey,
            value: str,
            debounce: true,
          }) as any
        )
      }
      onBlur={() =>
        dispatch(
          onFieldChanged({
            pluginName,
            key: field.stateKey,
            value: val,
            debounce: false,
          }) as any
        )
      }
    />
  );
}
