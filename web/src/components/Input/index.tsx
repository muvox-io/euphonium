import css from './Input.module.scss';

type InputProps = {
    tooltip: string;
    value: string;
    onChange: (value: string) => void;
}
  
export default ({tooltip, value, onChange} : InputProps) => {
    return (<div>
        <div class={css.input__tooltip}>{tooltip}</div>
        <input className={css.input} value={value} onChange={(e: any) => onChange(e.target.value)}></input>
    </div>)
}