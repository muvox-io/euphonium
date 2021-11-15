import css from "./Button.module.scss";

type ButtonProps = {
    onClick: () => void;
}

export default ({ onClick } : ButtonProps) => {
        return (
            <button className={css.button} onClick={(e) => onClick()}>Save changes</button>
        )
}