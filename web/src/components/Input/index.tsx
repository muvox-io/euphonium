import Icon from "../Icon";

type InputProps = {
  tooltip?: string;
  value?: string;
  icon?: string;
  type?: string;
  placeholder?: string;
  onChange?: (value: string) => void;
  onSubmit?: (value: string) => void;
};

export default ({
  tooltip = "",
  value = "",
  type = "text",
  onChange,
  onSubmit,
  icon,
  placeholder,
}: InputProps) => {
  return (
    <div class='lg:min-w-[400px] min-w-full relative'>
      <div class='text-gray-400 text-s mb-1'>{tooltip}</div>
      <input
        placeholder={placeholder}
        className={`${icon ? "pl-10" : ""} bg-gray-600 p-3 rounded-xl min-w-full`}
        value={value}
        type={type}
        onChange={(e: any) => onSubmit && onSubmit(e.target.value)}
        onInput={(e: any) => onChange && onChange(e.target.value)}
      ></input>
      {icon && (
        <div class='text-gray-400  left-1 top-3 absolute'>
          <Icon name="search" />
        </div>
      )}
    </div>
  );
};
