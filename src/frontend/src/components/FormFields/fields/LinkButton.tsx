import { ConfigurationFieldLinkButton } from "../../../api/euphonium/plugins/models";
import Button from "../../ui/Button";
import Input from "../../ui/Input";
import { FieldProps } from "./FieldProps";

export default function LinkButton({
  field,
}: FieldProps<ConfigurationFieldLinkButton>) {
  return (
    <div class="min-w-full flex flex-col items-start lg:flex-row lg:items-center">
      <div class="text-app-text-secondary font-thin text-l mb-2 mt-2">
        {field.label}
      </div>
      <div class="grow" style={{flexGrow: '1'}}>{/** fucking tailwind */}</div>
      <Button
        elementType="a"
        href={field.link}
        class="self-stretch lg:self-end lg:ml-auto"
        rel="noopener noreferrer"
        target="_blank"
      >
        {field.placeholder}
      </Button>
    </div>
  );
}
