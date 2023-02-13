export default function ({ header = "", children = {} }) {
  return (
    <div class="fixed top-0 bottom-0 bg-black/50 left-0 m-0 right-0 w-screen h-screen z-50 flex justify-center items-center border-app-border border-1">
      <div class="bg-app-primary lg:h-auto text-2xl text-app-text-primary pr-8 pt-6 pb-6 pl-8 lg:rounded-[40px] m-auto flex flex-col">
        {header}
        {children}
      </div>
    </div>
  );
}
