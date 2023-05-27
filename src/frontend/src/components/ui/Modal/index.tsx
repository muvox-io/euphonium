export default function ({
  header = "",
  children = {},
  onBackdropClick = () => { },
}) {
  return (
    <div class="fixed top-0 bottom-0 left-0 m-0 pt-10 pb-10 right-0 w-screen h-screen z-50 flex justify-center items-center border-app-border border-1">
      <div
        class="bg-black/50 fixed  top-0 bottom-0 left-0 m-0 right-0 z-20"
        onClick={onBackdropClick}
      />
      <div class="bg-app-primary max-h-full font-light text-lg text-app-text-secondary pr-10 pt-8 pb-8 pl-10 lg:rounded-[40px] m-auto flex flex-col z-30">
        <p class="mb-6 text-app-text-primary text-xl font-normal">{header}</p>
        <div class="overflow-y-auto overflow-x-hidden">
          {children}
        </div>
      </div>
    </div>
  );
}
