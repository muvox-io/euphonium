import {
    ConfigurationParagraph
} from "../../../api/euphonium/plugins/models";
import { FieldProps } from "./FieldProps";

export default function Paragraph({
  field,
}: FieldProps<ConfigurationParagraph>) {
  return <p>{field.text}</p>;
}
