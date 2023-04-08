import { useState } from "preact/hooks";
import { useDispatch } from "react-redux";
import { ConfigurationButtonField } from "../../../api/euphonium/plugins/models";
import { onFieldEvent } from "../../../redux/reducers/pluginConfigurationsReducer";
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
  pluginName,
}: FieldProps<ConfigurationButtonField>) {
  const [isLoading, setIsLoading] = useState(false);
  const dispatch = useDispatch();
  const onClicked = async () => {
    try {
      setIsLoading(true);
      await dispatch(
        onFieldEvent({
          pluginName,
          event: {
            fieldId: field.id,
            name: "click",
            payload: "",
          },
        }) as any
      );
    } catch (e) {
      console.error(e);
    } finally {
      setIsLoading(false);
    }
  };
  return (
    <div class="min-w-full flex flex-col items-start lg:flex-row lg:items-center">
      <div class="text-app-text-secondary font-thin text-l mb-2 mt-2">
        {field.label}
      </div>
      <div class="grow" style={{ flexGrow: "1" }}>
        {/** fucking tailwind */}
      </div>
      <Button class="self-stretch lg:self-end lg:ml-auto" onClick={onClicked}>
        {field.label || field.buttonText}
      </Button>
    </div>
  );
}
