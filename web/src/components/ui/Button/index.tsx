import "./Button.scss";

export interface ButtonProps extends JSX.HTMLAttributes<HTMLButtonElement> {}

export default ({ ...rest }: ButtonProps) => {
  return <button {...rest} class={`button ${rest.class || ""}`} />;
};
