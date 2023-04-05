import "./Button.scss";

export interface ButtonProps extends JSX.HTMLAttributes<HTMLButtonElement> {
  kind?: "normal" | "primary" | "borderless";
  progress?: number;
  elementType?: keyof JSX.IntrinsicElements;
  loadingState?: boolean;
}

export default ({
  elementType = "button",
  kind = "normal",
  loadingState = false,
  ...rest
}: ButtonProps) => {
  let ElementType = elementType as any;
  return (
    <ElementType
      {...rest}
      class={`button  ${kind} ${loadingState ? "loading" : ""} ${
        rest.class || ""
      }`}
    />
  );
};
