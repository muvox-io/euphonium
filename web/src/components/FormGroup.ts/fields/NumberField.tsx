import Input from "../../ui/Input";
import { FieldProps } from "./FieldProps";

export default function NumberField({
  field,
  value,
  onChange,
}: FieldProps<string>) {
  return (
    <Input
      type="number"
      tooltip={field.label}
      value={value}
      onChange={(str) => onChange(str)}
    />
  );
}
