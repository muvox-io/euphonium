type CheckboxProps = {
  label: string;
  value: boolean;
  onChange: (value: boolean) => void;
};

export default ({ label = "", value, onChange }: CheckboxProps) => {
  return (
    <div class="min-w-full flex flex-row items-center">
      <div class="text-app-text-secondary font-light text-m">
        {label}
      </div>
      <input
        onChange={(e: any) => {
          onChange(e.target.checked);
        }}
        checked={value!!}
        id="checkbox"
        aria-describedby="checkbox"
        type="checkbox"
        class="bg-app-secondary h-[20px] md:h-[40px] p-3 rounded-xl min-w-[20px] lg:self-auto ml-auto"
      />
    </div>
  );
};
