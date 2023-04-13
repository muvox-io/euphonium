import { useDispatch, useSelector } from "react-redux";
import { SelectFieldConfiguration } from "../../../api/euphonium/plugins/models";
import {
  onFieldChanged,
  pluginStateSelector,
} from "../../../redux/reducers/pluginConfigurationsReducer";
import Select from "../../ui/Select";
import { FieldProps } from "./FieldProps";

export default function SelectField({
  field,
  pluginName,
}: FieldProps<SelectFieldConfiguration>) {
  const pluginState = useSelector(pluginStateSelector(pluginName));
  const dispatch = useDispatch();
  return (
    <Select
      tooltip={field.label!!}
      value={pluginState[field.stateKey]!!}
      onChange={(str) => {
        dispatch(
          onFieldChanged({
            pluginName,
            key: field.stateKey,
            value: str,
            debounce: false,
          }) as any
        );
      }}
      values={field.values!!}
    />
  );
}
