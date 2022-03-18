type SelectProps = {
  tooltip: string;
  values: string[];
  value: string;
  onChange: (value: string) => void;
}

export default ({ tooltip = "", values = [], value, onChange }: SelectProps) => {
  return (
    <div>
      <div class='text-app-text-secondary font-thin text-[14px] mb-2 '>{tooltip}</div>
      <div class='bg-app-secondary rounded-xl pr-2'>
        <select class='bg-transparent p-3 h-[45px] w-full' value={value} onChange={(v: any) => onChange(v.target.value)}>
          {values.map((result, index) => (
            <option class='bg-app-secondary' value={result}>{result}</option>
          ))}
        </select>
      </div>
    </div>
  );
};
