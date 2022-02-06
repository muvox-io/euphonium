import Editor, { loader } from "@monaco-editor/react";
import { useContext, useEffect, useState } from "preact/hooks";
import { getFileContent } from "../../api/api";
import { StateContext } from "../utils/state-context";
import getBerrySyntax from './berry';

loader.init().then((monaco) => {
  monaco.languages.register({ id: "berry" });
  monaco.languages.setMonarchTokensProvider("berry", getBerrySyntax());
});

const WELCOME_MESSAGE = `#
# Welcome to Euphonium Web Editor.
#
# - What can I do with this?
#
#   This editor allows you to connect to your Euphonium instance and change it's code on the fly.
#
`;

export default function ({ file = "" }) {
  const [isDirty, setIsDirty] = useContext(StateContext).isDirty;
  const [fileContent, setFileContent] = useContext(StateContext).currentFileContent;
  const [fileName, setFileName] = useContext(StateContext).currentFileName;

  const loadFile = async (file: any) => {
    if (!file) {
      setFileContent(WELCOME_MESSAGE);
    } else {
      setIsDirty(false);
      setFileContent("Loading file...");
      setFileName(file);
      setFileContent(await getFileContent(file));
    }
  };

  useEffect(() => {
    loadFile(file);
  }, [file]);
  return (
    <Editor
      height="75vh"
      width="calc(100vw - 220px)"
      theme="vs-dark"
      path="dd.be"
      defaultLanguage="berry"
      defaultValue={""}
      onChange={(value: any, ev: any) => { 
        if (!isDirty) {
          setIsDirty(true);
        }
        setFileContent(value);
       }}
      value={fileContent}
    />)
}
