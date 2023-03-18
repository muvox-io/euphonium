import Input from "../../ui/Input";
import { FieldProps } from "./FieldProps";

export default function TextField({
  field,
  value,
  onChange,
  onChangeFinished,
}: FieldProps<string>) {
  return (
    <Input
      tooltip={field.label}
      value={value}
      onChange={(str) => onChange(str)}
      onBlur={onChangeFinished}
    />
  );
}
