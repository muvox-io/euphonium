import { useEffect, useState } from "preact/hooks";
import eventSource from "../../../api/euphonium/eventSource";
import Icon from "../Icon";
import { useDispatch, useSelector } from "react-redux";
import { RootState } from "../../../redux/store";
import {
  NotificationInReducer,
  expireNotification,
} from "../../../redux/reducers/notificationsReducer";

const Nofitication = ({
  notification,
  onClose = () => {},
}: {
  notification: NotificationInReducer;
  onClose?: () => void;
}) => {
  const [progressStyle, setProgressStyle] = useState({
    transition: notification.expiryTimeout + "ms linear",
    width: "100%",
  });
  useEffect(() => {
    const t = setTimeout(() => {
      setProgressStyle({
        ...progressStyle,
        width: "0%",
      });
    }, 1);
    const expire = setTimeout(() => {
      onClose();
    }, notification.expiryTimeout);

    return () => {
      clearTimeout(t);
      clearTimeout(expire);
    };
  }, []);
  let color = "bg-notif-" + notification.type;
  return (
    <div
      class={`p-3 m-4 shadow-xl rounded-xl flex text-white flex-row ${color} relative overflow-hidden`}
    >
      <div class="mr-4 space-y-1">
        <div>{notification.message}</div>
        {notification.submessage && (
          <p class="text-xs text-bold">{notification.submessage}</p>
        )}
        <div class="text-xs text-opacity-50	italic">
          from {notification.source}
        </div>
      </div>
      <div class="ml-auto flex items-center cursor-pointer">
        <Icon onClick={onClose} name="close" />
      </div>
      <div
        class="absolute w-full h-1 bottom-0 left-0 bg-black bg-opacity-40"
        style={progressStyle}
      ></div>
    </div>
  );
};

export default function () {
  const notifications = useSelector(
    (state: RootState) => state.notifications.notifications
  );
  const dispatch = useDispatch();
  return (
    <div class="fixed flex flex-col right-0 top-0 z-50">
      {notifications.map((n) => (
        <Nofitication
          notification={n}
          onClose={() => {
            dispatch(expireNotification(n.id) as any);
          }}
        />
      ))}
    </div>
  );
}
