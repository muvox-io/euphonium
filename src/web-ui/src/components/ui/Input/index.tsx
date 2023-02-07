import useIsMobile from "../../../utils/isMobile.hook";
import Icon from "../Icon";

type InputProps = {
  tooltip?: string;
  value?: string;
  icon?: string;
  type?: string;
  placeholder?: string;
  width?: number;
  singleLine?: boolean;
  onChange?: (value: string) => void;
  onSubmit?: (value: string) => void;
  onBlur?: (value: string) => void;
};

export default ({
  tooltip = "",
  value = "",
  type = "text",
  singleLine,
  onChange,
  onSubmit,
  icon,
  width,
  onBlur,
  placeholder,
}: InputProps) => {
  const isMobile = useIsMobile();
  return (
    <div class="flex flex-col w-full items-start md:flex-row md:items-center">
      <div class="text-app-text-secondary font-light text-m mb-3 md:mb-0 md:mt-0">
        {tooltip}
      </div>
      <input
        placeholder={placeholder}
        className={`${
          icon ? "pl-10" : "" +
          width && !isMobile ? ` w-[${width}px]` : ""
        } bg-app-secondary appearance-none h-[45px] p-3 text-center rounded-xl self-stretch md:self-auto md:ml-auto`}
        value={value}
        type={type}
        onChange={(e: any) => onSubmit && onSubmit(e.target.value)}
        onInput={(e: any) => onChange && onChange(e.target.value)}
        onBlur={(e: any) => onBlur && onBlur(e.target.value)}
      ></input>
    </div>
  );
};
