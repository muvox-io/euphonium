export default ({ isSelected = false, children = {}, onClick = () => {} }) => {
    return (
        <div class="relative">
            <div onClick={(v) => onClick()} class='text-l md:text-sm font-thin bg-grey hover:bg-gray-600 text-white py-4 md:py-3 px-4 rounded-xl'>
                {isSelected ? (<div class="w-1 absolute right-0 top-0 bottom-0 -mr-6 bg-green-600"></div>) : null}
                {children}
            </div>
        </div>
    );
};
