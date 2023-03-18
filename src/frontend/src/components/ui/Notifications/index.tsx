import { useEffect, useState } from "preact/hooks";
import eventSource from "../../../api/euphonium/eventSource";
import Icon from "../Icon";

const Nofitication = ({
  source = "",
  message = "",
  submessage = "",
  type = "",
  onClose = () => {},
}) => {
  let color = "bg-notif-" + type;
  return (
    <div
      class={`p-3 m-4 shadow-xl rounded-xl flex text-white flex-row ${color}`}
    >
      <div class="mr-4 space-y-1">
        <div>{message}</div>
        {submessage && <p class="text-xs text-bold">{submessage}</p>}
        <div class="text-xs text-opacity-50	italic">from {source}</div>
      </div>
      <div class="ml-auto flex items-center">
        <Icon onClick={onClose} name="close" />
      </div>
    </div>
  );
};

export default function () {
  const [notifs, setNotifs] = useState<any[]>([]);

  useEffect(() => {
    eventSource.on("notification", ({ data }: any) => {
      setNotifs([...notifs, JSON.parse(data)]);
      setTimeout(() => {
        setNotifs(notifs.slice(1));
      }, 3000);
    });
  }, []);
  return (
    <div class="fixed absolute flex flex-col right-0 top-0 z-50">
      {notifs.map((n) => (
        <Nofitication
          {...n}
          onClose={() => {
            setNotifs(notifs.filter((notif) => notif !== n));
          }}
        />
      ))}
    </div>
  );
}
