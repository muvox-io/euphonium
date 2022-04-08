import "./Button.scss";

export interface ButtonProps extends JSX.HTMLAttributes<HTMLButtonElement> {
  progress?: number;
  elementType?: keyof JSX.IntrinsicElements;

}

export default ({ elementType = "button", ...rest }: ButtonProps) => {
  let ElementType = elementType as any;
  return <ElementType {...rest} class={`button ${rest.class || ""}`} />;
};
