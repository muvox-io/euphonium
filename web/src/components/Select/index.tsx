import css from "./Select.module.scss";

type SelectProps = {
  tooltip: string;
  values: string[];
  value: string;
  onChange: (value: string) => void;
}

export default ({ tooltip = "", values = [], value, onChange }: SelectProps) => {
  return (
    <div>
      <div class={css.select__tooltip}>{tooltip}</div>
      <div class={css.select__wrapper}>
        <select class={css.select} value={value} onChange={(v: any) => onChange(v.target.value)}>
          {values.map((result, index) => (
            <option value={result}>{result}</option>
          ))}
        </select>
      </div>
    </div>
  );
};
