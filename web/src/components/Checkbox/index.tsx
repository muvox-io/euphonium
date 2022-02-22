import Icon from "../Icon";

type CheckboxProps = {
    label: string;
    value: boolean;
    onChange: (value: boolean) => void;
};

export default ({
    label = "",
    value,
    onChange
}: CheckboxProps) => {
    return (
        <div class='lg:min-w-[400px] min-w-full relative'>
            <div class="flex items-start items-center mt-3 mb-1">
                <input 
                    onChange={(e:any) => {onChange(e.target.checked)}}
                    checked={value!!}
                    id="checkbox" aria-describedby="checkbox" type="checkbox" class="bg-gray-50 border-gray-300 focus:ring-3 focus:ring-blue-300 h-4 w-4 rounded" />
                <label for="checkbox" class="text-sm ml-3 font-medium text-app-text-secondary">{label}</label>
            </div>

        </div>
    );
};
