import { useDispatch, useSelector } from "react-redux";
import { ConfigurationFieldText } from "../../../api/euphonium/plugins/models";
import { onFieldChanged, pluginStateSelector } from "../../../redux/reducers/pluginConfigurationsReducer";
import Input from "../../ui/Input";
import { FieldProps } from "./FieldProps";

export default function TextField({ field, pluginName }: FieldProps<ConfigurationFieldText>) {
  const pluginState = useSelector(pluginStateSelector(pluginName));
  const dispatch = useDispatch();
  return (
    <Input
      tooltip={field.label}
      value={pluginState[field.stateKey]}
      onChange={(str) => dispatch(onFieldChanged({
        pluginName,
        key: field.stateKey,
        value: str,
        debounce: true
      }) as any)}
      onBlur={() => dispatch(onFieldChanged({
        pluginName,
        key: field.stateKey,
        value: pluginState[field.stateKey],
        debounce: false
      }) as any)}
    />
  );
}
