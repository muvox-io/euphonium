import "./Button.scss";

export interface ButtonProps extends JSX.HTMLAttributes<HTMLButtonElement> {
  progress?: number;
}

export default ({ ...rest }: ButtonProps) => {
  return <button {...rest} class={`button ${rest.class || ""}`} />;
};
