type SelectProps = {
  tooltip: string;
  values: string[];
  value: string;
  onChange: (value: string) => void;
}

export default ({ tooltip = "", values = [], value, onChange }: SelectProps) => {
  return (
    <div>
      <div class='text-app-text-secondary text-s mb-1'>{tooltip}</div>
      <div class='bg-app-secondary border border-app-border rounded-xl pr-2'>
        <select class='bg-transparent p-3 w-full' value={value} onChange={(v: any) => onChange(v.target.value)}>
          {values.map((result, index) => (
            <option value={result}>{result}</option>
          ))}
        </select>
      </div>
    </div>
  );
};
