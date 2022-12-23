import Icon from "../Icon";

type InputProps = {
  tooltip?: string;
  value?: string;
  icon?: string;
  type?: string;
  height?: number;
  placeholder?: string;
  onChange?: (value: string) => void;
  onSubmit?: (value: string) => void;
  onBlur?: (value: string) => void;
  class: string;
};

export default ({
  tooltip = "",
  value = "",
  type = "text",
  onChange,
  onSubmit,
  icon,
  height = 45,
  onBlur,
  placeholder,
  ...rest
}: InputProps) => {
  return (
    <div class={`min-w-full flex flex-col items-start lg:flex-row lg:items-center ${rest.class}`}>
      <div class="text-app-text-secondary font-thin text-l mb-2 mt-2">
        {tooltip}
      </div>
      <input
        placeholder={placeholder}
        className={`${
          icon ? "pl-10" : ""
        } bg-app-secondary w-full p-3 rounded-xl min-w-[420px] self-stretch lg:self-auto lg:ml-auto`}
        value={value}
        type={type}
        style={{ height: `${height}px` }}
        onChange={(e: any) => onSubmit && onSubmit(e.target.value)}
        onInput={(e: any) => onChange && onChange(e.target.value)}
        onBlur={(e: any) => onBlur && onBlur(e.target.value)}
      ></input>
    </div>
  );
};
