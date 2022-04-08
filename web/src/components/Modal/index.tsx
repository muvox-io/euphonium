export default function ({ header = "", children = {} }) {
  return (
    <div class="fixed top-0 bottom-0 bg-black/50 left-0 m-0 right-0 w-screen h-screen z-50 flex justify-center items-center border-app-border border-1">
      <div class="bg-app-primary md:min-w-[600px] rounded-2xl p-7">
        <div class="text-2xl mb-3">{header}</div>

        {children}
      </div>
    </div>
  );
}
