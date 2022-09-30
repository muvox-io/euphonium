import { useState } from "preact/hooks";
import Card from "../../components/ui/Card";
import Icon from "../../components/ui/Icon";

const PACKAGES = [
  {
    name: "Core support package",
    version: "v0.1.0",
    description: "Euphonium is a modular audio player for the Raspberry Pi",
  },
  {
    name: "ESP32 support package",
    version: "v0.1.0",
    description: "Euphonium is a modular audio player for the Raspberry Pi",
  },
];

const Package = ({ pkg }: any) => {
  const [expanded, setExpanded] = useState(false);
  return (
    <div
      onClick={() => {
        setExpanded(!expanded);
      }}
      className="flex flex-col bg-app-primary p-3 rounded-xl transition-transform mb-4 cursor-pointer"
    >
      <div className="flex flex-row  items-center transition">
        <div className="text-l mr-2">{pkg.name}</div>
        <div className="text-sm text-app-text-secondary mt-[1px]">
          {pkg.version}
        </div>
        <div className="text-[24px] h-[24px] w-[24px] mt-[-10px] ml-auto mr-4">
          <Icon name={expanded ? "arrow-down" : "arrow-right"}></Icon>
        </div>
      </div>
      {expanded ? (
        <div className="transition text-sm text-app-text-secondary mt-2">
          {pkg.description}
        </div>
      ) : null}
    </div>
  );
};

export default () => {
  return (
    <Card title={"Package manager"} subtitle={"manage installed packages"}>
      <div className="mt-6">
        {PACKAGES.map((pkg) => (
          <Package pkg={pkg} />
        ))}
      </div>
    </Card>
  );
};
