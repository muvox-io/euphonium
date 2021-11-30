type SelectProps = {
  tooltip: string;
  values: string[];
  value: string;
  onChange: (value: string) => void;
}

export default ({ tooltip = "", values = [], value, onChange }: SelectProps) => {
  return (
    <div>
      <div class='text-gray-400 text-s mb-1'>{tooltip}</div>
      <div class='bg-gray-600 rounded-xl pr-2'>
        <select class='bg-transparent p-3 w-full' value={value} onChange={(v: any) => onChange(v.target.value)}>
          {values.map((result, index) => (
            <option value={result}>{result}</option>
          ))}
        </select>
      </div>
    </div>
  );
};
