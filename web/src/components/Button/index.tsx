type ButtonProps = {
    onClick: () => void;
    disabled?: boolean;
    type?: 'transparent' | 'primary';
    children: any;
}

export default ({ onClick, disabled = false, type = 'transparent', children }: ButtonProps) => {
    if (type == 'transparent') {
        return (
            <button className={` ${(disabled && 'text-gray-400') || 'text-red-500'}`} disabled={disabled} onClick={(e) => onClick()}>{ children }</button>
        )
    }

    return (
        <button disabled={disabled} className={`${(disabled && 'bg-gray-400') || 'bg-green-600'} font-bold p-3 rounded-xl w-full`} onClick={(e) => onClick()}>{ children }</button>
    )
}
