import { AnyComponent, FunctionComponent } from "preact";
import { useSelector } from "react-redux";
import {
  ConfigurationField,
  ConfigurationFieldType
} from "../../api/euphonium/plugins/models";
import { pluginStateReadySelector } from "../../redux/reducers/pluginConfigurationsReducer";
import useIsMobile from "../../utils/isMobile.hook";
import Separator from "../ui/Separator/Separator";
import ButtonField from "./fields/ButtonField";
import CheckboxField from "./fields/CheckboxField";
import { FieldProps } from "./fields/FieldProps";
import FormGroup from "./fields/FormGroup";
import LinkButton from "./fields/LinkButton";
import ModalConfirm from "./fields/ModalConfirm";
import ModalGroup from "./fields/ModalGroup";
import NumberField from "./fields/NumberField";
import SelectField from "./fields/SelectField";
import TextField from "./fields/TextField";
import Paragraph from "./fields/Paragraph";
import Table from "./fields/Table";
import FileUploadField from "./fields/FileUploadField";

export interface FormFieldsProps {
  pluginName: string;
  fields: ConfigurationField[];
 
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
  [ConfigurationFieldType.MODAL_GROUP]: ModalGroup,
  [ConfigurationFieldType.PARAGRAPH]: Paragraph,
  [ConfigurationFieldType.TABLE]: Table,
  [ConfigurationFieldType.FILE_UPLOAD_FIELD]: FileUploadField,
};

export default function FormFields({
  fields,
  pluginName
}: FormFieldsProps) {
  const isMobile = useIsMobile();
  const isPluginStateReady = useSelector(pluginStateReadySelector(pluginName));
  if(!isPluginStateReady) return null; // wait for plugin state to be ready between receiving the response and applying the state via redux
  return (
    <div class="flex flex-col md:space-y-5">
      {fields
        .filter((e) => !(e as any).hidden) // TODO: fix typing
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
                  key={field.id}
                  field={field}
                  pluginName={pluginName}
                />
              </div>
              {isMobile ? <Separator /> : null}
            </>
          );
        })}
    </div>
  );
}
