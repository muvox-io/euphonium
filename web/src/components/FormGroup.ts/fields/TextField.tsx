import Input from "../../ui/Input";
import { FieldProps } from "./FieldProps";

export default function TextField({
  field,
  value,
  onChange,
}: FieldProps<string>) {
  return (
    <Input
      tooltip={field.label}
      value={value}
      onChange={(str) => onChange(str)}
    />
  );
}
