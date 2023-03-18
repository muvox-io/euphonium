type SelectProps = {
  tooltip: string;
  values: string[];
  value: string;
  onChange: (value: string) => void;
};

export default ({
  tooltip = "",
  values = [],
  value,
  onChange,
}: SelectProps) => {
  return (
    <div class="min-w-full flex flex-row items-center">
      <div class="text-app-text-secondary font-light text-m">
        {tooltip}
      </div>
      <div class="bg-app-secondary rounded-xl pr-2 ml-auto">
        <select
          class="bg-transparent p-3 h-[45px] w-full text-center"
          value={value}
          onChange={(v: any) => onChange(v.target.value)}
        >
          {values.map((result, index) => (
            <option
              class="bg-app-secondary"
              value={result}
              selected={result == value}
            >
              {result}
            </option>
          ))}
        </select>
      </div>
    </div>
  );
};
