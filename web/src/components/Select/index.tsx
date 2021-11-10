import css from "./Select.module.scss";

type SelectProps = {
  tooltip: string;
  values: string[];
  value: string;
}

export default ({ tooltip = "", values = [], value }: SelectProps) => {
  return (
    <div>
      <div class={css.select__tooltip}>{tooltip}</div>
      <div class={css.select__wrapper}>
        <select class={css.select}>
          {<option value="0">{value}</option>}
          {values.map((result, index) => (
            <option value={index+1}>{result}</option>
          ))}
        </select>
      </div>
    </div>
  );
};
