import { ConfigurationModalGroup } from "../../../api/euphonium/plugins/models";
import { ModalType } from "../../../redux/reducers/modalsReducer";
import ModalTrigger from "../../ModalTrigger";
import { FieldProps } from "./FieldProps";

/**
 * A field that appears as a modal
 * @param param0
 * @returns
 */
export default function ModalGroup({
  field,
  pluginName,
}: FieldProps<ConfigurationModalGroup>) {
  return (
    <ModalTrigger
      type={ModalType.FORM_MODAL}
      id={pluginName + "." + field.id}
      dismissable={field.dismissable}
      priority={field.priority}
      title={field.title}
      data={{
        pluginName,
        fields: field.children,
        fieldId: field.id,
      }}
    />
  );
}
