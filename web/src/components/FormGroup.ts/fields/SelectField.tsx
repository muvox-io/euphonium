import Select from "../../ui/Select";
import { FieldProps } from "./FieldProps";

export default function SelectField({
  field,
  value,
  onChange,
}: FieldProps<string>) {
  return (
    <Select
      tooltip={field.label!!}
      value={value}
      onChange={(str) => onChange(str)}
      values={field.values!!}
    />
  );
}
