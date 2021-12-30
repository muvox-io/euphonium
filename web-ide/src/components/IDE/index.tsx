import { useState, useEffect, useRef } from "preact/hooks";
import Editor from "../Editor";
import FileItem from "../FileItem";
import FileTree from "../FileTree";

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

export default function ({ipAddr = ""}) {
  const [file, setFile] = useState<string>("");
  const [selectedFile, setSelectedFile] = useState<string>("");
  const [fileContent, setFileContent] = useState<string>("");
  const [logs, setLogs] = useState<string[]>([]);
  const endOfLogs = useRef(null);
  const loadFile = async (file: any) => {
    const response = await fetch("http://" + ipAddr + "/file" + file);
    const json = await response.text();
    setFile(json);
    setSelectedFile(file);
  };

  const loadLogs = async () => {
    const response = await fetch("http://" + ipAddr +  "/request_logs");
    const json = await response.text();
    setLogs(json.split("\n"));
    setTimeout(() => {
      (endOfLogs?.current as any).scrollIntoView({ behavior: "smooth" });
    }, 10);
  };

  useEffect(() => {
    loadLogs();
    let id = setInterval(() => loadLogs(), 1000);
    return () => clearInterval(id);
  }, []);

  const updateFile = async (selectedFile: any) => {
    const response = await fetch("http://" + ipAddr + "/file" + selectedFile, {
      method: "POST",
      body: fileContent,
    });
    await response.text();
  };
  return (
    <div class="flex overflow-auto h-screen w-screen flex-col bg-app-secondary">
      <div class="flex flex-row">
        <div class="text-white text-xl m p-3 font-bold flex bg-app-primary h-screen flex-col w-[220px] h-screen">
          Euphonium
          <div class="text-app-text-secondary text-sm font-light mb-3">
            web editor
          </div>
          <input
            value={ipAddr}
            class="bg-app-secondary text-sm p-2 mb-3 rounded-xl"
          ></input>
          <button class="text-sm bg-blue-600 p-1 rounded-xl mb-3 shadow-xl border-app-border border">
            connected
          </button>
          <div class="h-[0.5px] bg-gray-600 mb-3 mt-2" />
          <FileTree ipAddr={ipAddr} fileSelected={(file) => loadFile(file)} />
          <div class="mt-auto text-app-text-secondary text-xs text-center mb-2">
            changes visible after reboot
          </div>
          <button
            onClick={(ev) => updateFile(selectedFile)}
            class="bg-green-600 p-1 rounded-full text-sm items-center align-center justify-center flex shadow-xl border-app-border border"
          >
            update file
          </button>
          <button
            onClick={(ev) => updateFile(selectedFile)}
            class="bg-red-600 mt-2 p-1 rounded-full text-sm items-center align-center justify-center flex shadow-xl border-app-border border"
          >
            reboot device
          </button>

        </div>
        <div class="flex flex-col">
          <Editor value={file} valueChanged={setFileContent} />
          <div class="h-[25vh]">
            <div class="text-app-text-secondary h-[5vh] text-sm p-3">
              System logs
            </div>
            <div class="bg-gray-900 p-3 h-[20vh] text-white text-xs overflow-auto font-mono pb-3 flex-col flex">
              {logs.map((e) => (
                <LogItem log={e} />
              ))}
              <span ref={endOfLogs}></span>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
}
