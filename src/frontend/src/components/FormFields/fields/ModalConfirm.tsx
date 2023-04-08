import Modal from "../../ui/Modal";
import Button from "../../ui/Button";
import { FieldProps } from "./FieldProps";

export default function ModalConfirm({
  field,
  value,
  onChange,
  onChangeFinished,
}: FieldProps<boolean | string>) {
  let okValue = field.okValue || true;
  let cancelValue = field.cancelValue || false;
  if (value === okValue || value === cancelValue) {
    return null;
  }
  return (
    <Modal header={field.label}>
      <p>{field.hint}</p>
      <div className="flex flex-col mt-10">
        <Button
          onClick={() => {
            onChange(okValue);
            onChangeFinished && onChangeFinished();
          }}
          kind="primary"
        >
          OK
        </Button>
        <Button
          onClick={() => {
            onChange(cancelValue);
            onChangeFinished && onChangeFinished();
          }}
          kind="borderless"
        >
          Cancel
        </Button>
      </div>
    </Modal>
  );
}
