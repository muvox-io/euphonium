import Input from "../../ui/Input";
import { FieldProps } from "./FieldProps";

export default function NumberField({
  field,
  value,
  onChange,
  onChangeFinished,
}: FieldProps<number>) {
  return (
    <Input
      type="number"
      tooltip={field.label}
      width={90}
      value={value.toString()}
      singleLine={true}
      onChange={(str) => onChange(parseFloat(str))}
      onBlur={onChangeFinished}
    />
  );
}
