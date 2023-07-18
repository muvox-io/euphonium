import { useState } from "preact/hooks";
import Button from "../Button";
import "./UploadButton.scss";

export interface UploadButtonProps
  extends JSX.HTMLAttributes<HTMLInputElement> {
  buttonText?: string;
}

export default ({ buttonText, ...rest }: UploadButtonProps) => {
  const [randomId] = useState((Math.random() * 10000).toString())[0];
  return (
    <div>
      <input id={randomId} class="hidden" type="file" {...rest} />
      <label for={randomId}>
        <Button elementType="span">{buttonText}</Button>
      </label>
    </div>
  );
};
