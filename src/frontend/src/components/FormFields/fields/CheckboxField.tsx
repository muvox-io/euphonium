import { FieldProps } from "./FieldProps";
import Checkbox from "../../ui/Checkbox";
import { CheckboxFieldConfiguration } from "../../../api/euphonium/plugins/models";
import { useDispatch, useSelector } from "react-redux";
import {
  onFieldChanged,
  pluginStateSelector,
} from "../../../redux/reducers/pluginConfigurationsReducer";

export default function CheckboxField({
  pluginName,
  field,
}: FieldProps<CheckboxFieldConfiguration>) {
  const pluginState = useSelector(pluginStateSelector(pluginName));
  const dispatch = useDispatch();
  return (
    <Checkbox
      label={field.label || "???"}
      value={pluginState[field.stateKey] == true}
      onChange={(v) => {
        dispatch(
          onFieldChanged({
            pluginName,
            key: field.stateKey,
            value: v,
            debounce: false,
          }) as any
        );
      }}
    />
  );
}
