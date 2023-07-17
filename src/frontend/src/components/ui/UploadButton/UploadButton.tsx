import "./UploadButton.scss";

export interface UploadButtonProps
  extends JSX.HTMLAttributes<HTMLInputElement> {}

export default ({ ...rest }: UploadButtonProps) => {
  return (
    <input
      type="file"
      {...rest}
    />
  );
};
