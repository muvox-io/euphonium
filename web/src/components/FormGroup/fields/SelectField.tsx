import Select from "../../ui/Select";
import { FieldProps } from "./FieldProps";

export default function SelectField({
  field,
  value,
  onChange,
  onChangeFinished,
}: FieldProps<string>) {
  return (
    <Select
      tooltip={field.label!!}
      value={value}
      onChange={(str) => {
        onChange(str);
        onChangeFinished && onChangeFinished();
      }}
      values={field.values!!}
    />
  );
}
