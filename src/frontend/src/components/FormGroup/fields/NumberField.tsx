import Input from "../../ui/Input";
import { FieldProps } from "./FieldProps";

export default function NumberField({
  field,
  value,
  onChange,
  onChangeFinished,
}: FieldProps<string>) {
  return (
    <Input
      type="number"
      tooltip={field.label}
      width={90}
      value={value}
      singleLine={true}
      onChange={(str) => onChange(str)}
      onBlur={onChangeFinished}
    />
  );
}
