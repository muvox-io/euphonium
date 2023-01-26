import Icon from "../Icon";

export default ({children, iconName, label}: any) => {
    return (<div class="w-full bg-app-primary mt-8 md:p-4 rounded-2xl flex flex-col">
    <div class="flex flex-row text-bold items-center text-l text-white">
      <span class="text-2xl mb-[-2px]">
        <Icon name={iconName}></Icon>
      </span>
      {label}
    </div>
    <div class="p-3">
    {children}
    </div>
  </div>);
}
