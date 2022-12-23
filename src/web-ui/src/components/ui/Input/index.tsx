import Icon from "../Icon";

type InputProps = {
  tooltip?: string;
  value?: string;
  icon?: string;
  type?: string;
  large?: boolean;
  placeholder?: string;
  onChange?: (value: string) => void;
  onSubmit?: (value: string) => void;
  onBlur?: (value: string) => void;
};

export default ({
  tooltip = "",
  value = "",
  type = "text",
  onChange,
  onSubmit,
  icon,
  large,
  onBlur,
  placeholder,
}: InputProps) => {
  return (
    <div class="min-w-full flex flex-col items-start lg:flex-row lg:items-center">
      <div class="text-app-text-secondary font-thin text-l mb-2 mt-2">
        {tooltip}
      </div>
      <input
        placeholder={placeholder}
        className={`${
          icon ? "pl-10" : ""
        } bg-app-secondary h-[45px] w-full p-3 rounded-xl min-w-[420px] self-stretch lg:self-auto lg:ml-auto`}
        value={value}
        type={type}
        onChange={(e: any) => onSubmit && onSubmit(e.target.value)}
        onInput={(e: any) => onChange && onChange(e.target.value)}
        onBlur={(e: any) => onBlur && onBlur(e.target.value)}
      ></input>
    </div>
  );
};
