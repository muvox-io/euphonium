import { useState, useEffect, useRef, useContext } from "preact/hooks";
import { StateContext } from "../utils/state-context";
import Editor from "../Editor";
import FileItem from "../FileItem";
import FileTree from "../FileTree";
import Router, { Route } from 'preact-router';
import Icon from "../Icon";
import SystemLogs from "../SystemLogs";
import { updateFileContent, restartSystem } from "../../api/api";


export default function () {
  const [isDirty, setIsDirty] = useContext(StateContext).isDirty;
  const [ipAddress, setIpAddress] = useContext(StateContext).address;
  const [fileName, setFileName] = useContext(StateContext).currentFileName;
  const [fileContent, setFileContent] = useContext(StateContext).currentFileContent;

  const doFileUpload = async () => {
    await updateFileContent(fileName, fileContent);
    setIsDirty(false);
  }

  return (
    <div class="flex overflow-auto h-screen w-screen flex-col bg-app-secondary">
      <div class="flex flex-row">
        <div class="text-white text-xl m p-3 font-bold flex bg-app-primary h-screen flex-col w-[220px] h-screen">
          Euphonium
          <div class="text-app-text-secondary text-sm font-light mb-3">
            web editor
          </div>
          <span class="text-blue-200 text-xs">see documentation</span>
          <div class="h-[0.5px] bg-gray-600 mb-3 mt-2" />
          <FileTree />
          <div class="mt-auto h-[0.5px] bg-gray-600 mb-3 mt-2" />
          <div class="mt-1 text-app-text-secondary text-xs text-center mb-2">
            <span class="text-blue-200">{ipAddress}</span> connected<br />
            changes visible after reboot
          </div>
          <div class="text-xs flex">
            <div class="cursor-pointer flex-row" onClick={async () => {
              await restartSystem();
            }}>
              <span class="text-red-600 text-xl mr-2">
                <Icon name="refresh" />
              </span>
              restart
            </div>
            {isDirty ? (<div onClick={() => {
              doFileUpload();
            }} class="cursor-pointer ml-auto items-center">
              <span class="ml-auto items-center text-green-600 text-xl mr-2">
                <Icon name="upload" />
              </span>
              upload file</div>) : (<div class="ml-auto items-center text-gray-400 ">
                <span class="text-xl mr-2 items-center">
                  <Icon name="upload" />
                </span>
                upload file</div>)}</div>
        </div>
        <div class="flex flex-col">
          <Router>
            <Route component={Editor} path="/:file*" />
          </Router>
          <SystemLogs />
        </div>
      </div>
    </div>
  );
}
