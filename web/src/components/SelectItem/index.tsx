export default ({ isSelected = false, children = {}, onClick = () => {} }) => {
    return (
        <div class="relative cursor-pointer">
            <div onClick={(v) => onClick()} class='text-l border border-app-border md:text-sm font-thin bg-app-secondary hover:bg-app-secondary-dark text-app-text-primary py-4 md:py-3 px-4 rounded-xl'>
                {isSelected ? (<div class="w-1 absolute right-0 top-0 bottom-0 -mr-6 bg-green-600"></div>) : null}
                {children}
            </div>
        </div>
    );
};
