import Button from "../../ui/Button";
import Input from "../../ui/Input";
import { FieldProps } from "./FieldProps";

/**
 * A field with a button that sets the value to true when the button is pressed.
 * @param param0
 * @returns
 */
export default function ButtonField({
  field,
  value,
  onChange,
  onChangeFinished,
}: FieldProps<boolean>) {
  return (
    <div class="min-w-full flex flex-col items-start lg:flex-row lg:items-center">
      <div class="text-app-text-secondary font-thin text-l mb-2 mt-2">
        {field.label}
      </div>
      <div class="grow" style={{ flexGrow: "1" }}>
        {/** fucking tailwind */}
      </div>
      <Button
        class="self-stretch lg:self-end lg:ml-auto"
        onClick={() => {
          onChange(true);
          if (onChangeFinished) {
            onChangeFinished();
          }
        }}
      >
        {field.label || field.buttonText}
      </Button>
    </div>
  );
}
