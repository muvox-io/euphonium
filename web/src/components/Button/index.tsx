import css from "./Button.module.scss";

export default (onClick: () => void) => {
        return (
            <button className={css.button} onClick={(e) => onClick()}>Save changes</button>
        )
}