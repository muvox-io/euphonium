import { useState, useEffect } from "preact/hooks";
import FileItem from "../FileItem";

export default function ({fileSelected = (a:any) => {}}) {
  const [files, setFiles] = useState<string[]>([]);
  const [selectedFile, setFile] = useState<string>("");

  useEffect(() => {
    const fetchFiles = async () => {
      const response = await fetch("http://192.168.1.234/directories");
      const json = await response.json();
      setFiles(json);
    };
    fetchFiles();
  }, []);

  return (
    <>
      <div class="text-app-text-secondary text-sm mb-3 font-light">
        script files
      </div>
      <div class="overflow-y-scroll overflow-x-hidden mb-2">
      {files.map((item) => (
      <div onClick={() =>{ setFile(item); fileSelected(item) }} >
        <FileItem name={item} isSelected={selectedFile == item} />
        </div>
      ))}
      </div>
    </>
  );
}
