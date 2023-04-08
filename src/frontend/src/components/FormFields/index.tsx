import { AnyComponent, FunctionComponent } from "preact";
import {
  ConfigurationField,
  ConfigurationFieldType,
} from "../../api/euphonium/plugins/models";
import useIsMobile from "../../utils/isMobile.hook";
import IconCard from "../ui/IconCard";
import Separator from "../ui/Separator/Separator";
import ButtonField from "./fields/ButtonField";
import CheckboxField from "./fields/CheckboxField";
import { FieldProps } from "./fields/FieldProps";
import FormGroup from "./fields/FormGroup";
import LinkButton from "./fields/LinkButton";
import ModalConfirm from "./fields/ModalConfirm";
import NumberField from "./fields/NumberField";
import SelectField from "./fields/SelectField";
import TextField from "./fields/TextField";

export interface FormGroupProps {
  fields: ConfigurationField[];
  value: any;
  onChange: (value: any) => void;
  onChangeFinished?: () => void;
}

export const FIELD_COMPONENTS: {
  [key in any]: FunctionComponent<FieldProps<any>>;
} = {
  [ConfigurationFieldType.TEXT]: TextField,
  [ConfigurationFieldType.NUMBER]: NumberField,
  [ConfigurationFieldType.CHECKBOX]: CheckboxField,
  [ConfigurationFieldType.SELECT]: SelectField,
  [ConfigurationFieldType.LINK_BUTTON]: LinkButton,
  [ConfigurationFieldType.MODAL_CONFIRM]: ModalConfirm,
  [ConfigurationFieldType.BUTTON_FIELD]: ButtonField,
  [ConfigurationFieldType.GROUP]: FormGroup,
};

export default function FormFields({
  fields,
  value,
  onChange,
  onChangeFinished,
}: FormGroupProps) {
  const isMobile = useIsMobile();
  console.log("value", value)
  return (
    <div class="flex flex-col md:space-y-5">
      {fields
        .filter((e) => !e.hidden)
        .map((field) => {
          const FieldComponent: AnyComponent<FieldProps<any>> =
            FIELD_COMPONENTS[field.type];
          if (!FieldComponent) {
            return <p>Unsupported field type: {field.type}</p>;
          }
          return (
            <>
              <div class="mt-4 mb-4 md:mt-0 md:mb-0">
                <FieldComponent
                  key={field.key}
                  field={field}
                  value={value}
                  onChange={(fieldValue: any) =>
                    onChange({ ...value, [field.key]: fieldValue })
                  }
                  onChangeFinished={onChangeFinished}
                />
              </div>
              {isMobile ? <Separator /> : null}
            </>
          );
        })}
    </div>
  );
}
