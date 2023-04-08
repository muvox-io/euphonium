import FormFields from "..";
import {
  ConfigurationField
} from "../../../api/euphonium/plugins/models";
import IconCard from "../../ui/IconCard";

export interface FormGroupProps {
  field: ConfigurationField;
  value: any;
  onChange: (value: any) => void;
  label?: string;
  onChangeFinished?: () => void;
}


export default function FormGroup({
  field,
  value,
  onChange,
  label,
  onChangeFinished,
}: FormGroupProps) {
 
  return (
    <IconCard iconName="settings" label={label}>
      <FormFields fields={field.children!} value={value} onChange={onChange} />
    </IconCard>
  );
}
