type CheckboxProps = {
  label: string;
  value: boolean;
  onChange: (value: boolean) => void;
};

export default ({ label = "", value, onChange }: CheckboxProps) => {
  return (
    <div class="min-w-full flex flex-col items-start lg:flex-row lg:items-center">
      <div class="text-app-text-secondary font-thin text-l mb-2 mt-2">
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
        class="bg-app-secondary h-[45px]  p-3 rounded-xl min-w-[20px] self-stretch lg:self-auto lg:ml-auto"
      />
    </div>
  );
};
