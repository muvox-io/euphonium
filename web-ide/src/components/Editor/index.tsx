import Editor, { loader } from "@monaco-editor/react";
import getBerrySyntax from './berry';

loader.init().then((monaco) => {
  // Register a new language
  monaco.languages.register({ id: "berry" });

  // Register a tokens provider for the language
  monaco.languages.setMonarchTokensProvider("berry", getBerrySyntax());
});

export default function ({value = "", valueChanged = (d: string) => {}}) {
    return (
      <Editor
        height="75vh"
        width="calc(100vw - 220px)"
        theme="vs-dark"
        path="dd.be"
        defaultLanguage="berry"
        defaultValue={value}
        onChange={(value: any, ev: any) => { valueChanged(value) }}
        value={value}
      />)
}
