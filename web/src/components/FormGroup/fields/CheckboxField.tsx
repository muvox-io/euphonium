import { FieldProps } from "./FieldProps";
import Checkbox from "../../ui/Checkbox";

export default function CheckboxField(props: FieldProps<string>) {
  return (
    <Checkbox
      label={props.field.label || "???"}
      value={props.value === "true"}
      onChange={(v) => {
        props.onChange(v ? "true" : "false");
        props.onChangeFinished && props.onChangeFinished();
      }}
    />
  );
}
