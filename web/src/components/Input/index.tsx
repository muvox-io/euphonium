import css from './Input.module.scss';

export default ({tooltip = "", value = ""}) => {
    return (<div>
        <div class={css.input__tooltip}>{tooltip}</div>
        <input className={css.input} value={value}></input>
    </div>)
}