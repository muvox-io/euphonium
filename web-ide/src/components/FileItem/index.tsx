export default function ({ name = "", isSelected = false }) {
  return (
    <div class="relative cursor-pointer">
      {isSelected ? (
        <div class="absolute right-0 top-0 bottom-0 w-1 bg-green-600 -mr-3 mb"></div>
      ) : null}
      <div class="font-light bg-app-secondary p-1 border-app-border border text-xs font-monospace mb-3">
        {name}
      </div>
    </div>
  );
}
