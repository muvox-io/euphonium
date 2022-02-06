import { useEffect, useRef, useState } from "preact/hooks";
import { getSystemLogs } from "../../api/api";

const LogItem = ({ log = "" }) => {
    const logType = log[0];
    let color = "bg-red-700";

    switch (logType) {
        case "I":
            color = "bg-green-700";
            break;
        case "E":
            color = "bg-red-700";
    }

    let tag = log.substring(log.indexOf("[") + 1, log.indexOf("]"))
    let file = log.substring(log.indexOf("] ") + 1, log.indexOf(": ") + 1);
    let logText = log.substring(log.indexOf(": ") + 1, log.length);

    return (
        <div class="flex flex-row items-center align-center mt-2">
            <span class={`${color} pl-2 pr-2 rounded-xl`}>{logType}</span> <span class="bg-blue-600 ml-2 p-[0.5px] pl-1 pr-1">{tag}</span><span class="ml-2 mr-2 text-app-text-secondary">{file}</span> <span class="">{logText}</span>
        </div>
    );
};

export default () => {
    const [logs, setLogs] = useState<string[]>([]);
    const endOfLogs = useRef(null);

    const loadLogs = async () => {
        const response = await getSystemLogs();
        setLogs(response.split("\n"));

        setTimeout(() => {
            (endOfLogs?.current as any).scrollIntoView({ behavior: "smooth" });
          }, 10);
    };

    useEffect(() => {
        loadLogs();
        let id = setInterval(() => loadLogs(), 1000);
        return () => clearInterval(id);
      }, []);

    return (<div class="h-[25vh]">
        <div class="text-app-text-secondary h-[5vh] text-sm p-3">
            System logs
        </div>
        <div class="bg-gray-900 p-3 h-[20vh] text-white text-xs overflow-auto font-mono pb-3 flex-col flex">
            {logs.map((e) => (
                <LogItem log={e} />
            ))}
            <span ref={endOfLogs}></span>
        </div>
    </div>);
}