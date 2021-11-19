import Icon from "../Icon";
import css from "./Input.module.scss";

type InputProps = {
  tooltip?: string;
  value: string;
  icon?: string;
  placeholder?: string;
  onChange: (value: string) => void;
};

export default ({
  tooltip,
  value,
  onChange,
  icon,
  placeholder,
}: InputProps) => {
  return (
    <div class={css.inputContainer}>
      {tooltip && <div class={css.input__tooltip}>{tooltip}</div>}

      <input
        placeholder={placeholder}
        className={`${css.inputContainer__input} ${icon && css.inputContainer__input_icon}`}
        value={value}
        onChange={(e: any) => onChange(e.target.value)}
      ></input>
      {icon && (
        <div class={css.inputContainer__icon}>
          <Icon name="search" />
        </div>
      )}
    </div>
  );
};
