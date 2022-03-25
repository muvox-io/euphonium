import Icon from "../Icon";

type NumberInputProps = {
  tooltip?: string;
  value?: string;
  placeholder?: string;
  onChange?: (value: string) => void;
  onSubmit?: (value: string) => void;
  onBlur?: (value: string) => void;
};

export default ({
  tooltip = "",
  value = "",
  onChange,
  onSubmit,
  onBlur,
  placeholder,
}: NumberInputProps) => {
  return (
    <div class='min-w-full flex flex-row items-center'>
      <div class='text-app-text-secondary font-thin text-l mb-2 mt-2'>{tooltip}</div>
      <input
        placeholder={placeholder}
        className={` bg-app-secondary h-[45px] pl-3 rounded-xl w-[80px] ml-auto text-center`}
        value={value}
        type='number'
        onChange={(e: any) => onSubmit && onSubmit(e.target.value)}
        onInput={(e: any) => onChange && onChange(e.target.value)}
        onBlur={(e: any) => onBlur && onBlur(e.target.value)}
      ></input>
    </div>
  );
};