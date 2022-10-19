import "./Button.scss";

export interface ButtonProps extends JSX.HTMLAttributes<HTMLButtonElement> {
  kind?: "normal" | "primary" | "borderless";
  progress?: number;
  elementType?: keyof JSX.IntrinsicElements;
}

export default ({ elementType = "button", kind = "normal", ...rest }: ButtonProps) => {
  let ElementType = elementType as any;
  return (
    <ElementType {...rest} class={`button  ${kind} ${rest.class || ""}`} />
  );
};
