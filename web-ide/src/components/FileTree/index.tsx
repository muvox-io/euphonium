import { route } from "preact-router";
import { useState, useEffect } from "preact/hooks";
import { getFileTree, renameFile, updateFileContent } from "../../api/api";
import FileItem from "../FileItem";
import Icon from "../Icon";

const FileEntry = ({ name = "", items = [], level = 0, fullPath = "", reloadCallback = () => {} }) => {
  const [open, setIsOpen] = useState<boolean>(false);

  var iconName = "file";
  if (items.length > 0 && open) {
    iconName = "folder-open";
  }

  if (items.length > 0 && !open) {
    iconName = "folder";
  }
  return (<div style={{ marginLeft: level * 8 }}>
    <div class="flex flex-row items-center cursor-pointer" onClick={(e) => {
      if (items.length) {
        setIsOpen(!open);
      } else {
        route(fullPath);
      }
    }}>
      {items.length > 0 ? (<div class={`text-[13px] -mr-1 text-[#fdb900]`}><Icon name={iconName} /></div>) : (<div class={`text-[13px] -mr-1 text-white`}><Icon name={iconName} /></div>)}

      <div class="font-light mt-1 ml-2 mb-1 p-1 text-xs font-monospace">
        {name}</div>
      <div class="ml-auto text-gray-300 text-xs cursor-pointer" onClick={async (ev) => {
        const newFilePath = fullPath.substring(0, fullPath.lastIndexOf("/"));

        if (items.length > 0) {
          const newFileName = prompt("Enter new file name\nThis file will be located in " + newFilePath);
          if (newFileName) await updateFileContent(newFilePath + "/" + newFileName, "");
        } else {
          const newFileName = prompt("Enter new name for file " + name);
          if (newFileName) await renameFile(fullPath, newFilePath + "/" + newFileName);
        }
        ev.stopImmediatePropagation();
        reloadCallback();
      }}>
        { items.length > 0 ? (<Icon name="plus" />) : (<Icon name="rename" />)} 
      </div>
    </div>
    {open && items.map((item, index) => <FileEntry key={index} {...item} level={level + 1} />)}
  </div>)
}

export default function () {
  const [fileTree, setFileTree] = useState<any>([]);
  const fetchFilesCallback = () => {
    const fetchFiles = async () => {
      setFileTree(await getFileTree());
    };
    fetchFiles();
  }
  useEffect(() => {
    fetchFilesCallback();
  }, []);

  return (
    <>
      <div class="text-app-text-secondary text-sm mb-3 font-light">
        script files
      </div>
      <div class="overflow-y-auto no-scrollbar overflow-x-hidden mb-2">
        {fileTree.map((item: any, index: number) => (
          <FileEntry key={index} {...item} reloadCallback={() => {
            fetchFilesCallback();
          }}/>
        ))}
      </div>
    </>
  );
}
