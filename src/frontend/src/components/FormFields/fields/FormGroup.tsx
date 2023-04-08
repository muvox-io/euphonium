import FormFields, { FormFieldsProps } from "..";
import {
  ConfigurationField,
  ConfigurationFieldGroup,
} from "../../../api/euphonium/plugins/models";
import IconCard from "../../ui/IconCard";
import { FieldProps } from "./FieldProps";

export default function FormGroup({
  field,
  pluginName,
}: FieldProps<ConfigurationFieldGroup>) {
  return (
    <IconCard iconName="settings" label={field.label}>
      <FormFields fields={field.children!} pluginName={pluginName} />
    </IconCard>
  );
}
