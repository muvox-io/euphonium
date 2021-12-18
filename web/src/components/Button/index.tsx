type ButtonProps = {
  onClick: () => void;
  disabled?: boolean;
  type?: "transparent" | "primary" | "progress";
  progress?: number;
  children: any;
};

export default ({
  onClick,
  disabled = false,
  type = "transparent",
  progress = 0,
  children,
}: ButtonProps) => {
  if (type == "transparent") {
    return (
      <button
        className={` ${
          (disabled && "text-app-text-secondary") || "text-red-500"
        }`}
        disabled={disabled}
        onClick={(e) => onClick()}
      >
        {children}
      </button>
    );
  }

  if (type == "progress") {
    return (
      <button
        disabled={disabled}
        className={`${
          (disabled && "bg-app-text-secondary") || "bg-green-500"
        } font-bold relative rounded-xl text-white w-full h-[45px]`}
        onClick={(e) => onClick()}
      >
        <div class="absolute top-0 left-0 right-0 bottom-0 bg-green-600 p-3 rounded-xl" style={{width: progress + '%', transition: 'width 2s'}} ></div>
        <div class="absolute top-0 left-0 right-0 bottom-0 flex justify-center items-center align-center">
          {children}
        </div>
      </button>
    );
  }

  return (
    <button
      disabled={disabled}
      className={`${
        (disabled && "bg-app-text-secondary") || "bg-green-500"
      } font-bold relative rounded-xl text-white w-full h-[45px]`}
      onClick={(e) => onClick()}
    >
      {children}
    </button>
  );
};
