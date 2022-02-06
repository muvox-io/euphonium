import { createContext } from 'preact';
import { useState } from 'preact/hooks';

export const StateContext = createContext<any>(null);

export default ({ children = {} }) => {

  const [address, setAddress] = useState('');
  const [isDirty, setIsDirty] = useState(false);
  const [currentFileContent, setCurrentFileContent] = useState('');
  const [currentFileName, setCurrentFileName] = useState('');

  const store = {
    address: [address, setAddress],
    isDirty: [isDirty, setIsDirty],
    currentFileContent: [currentFileContent, setCurrentFileContent],
    currentFileName: [currentFileName, setCurrentFileName]
  }

  return (<StateContext.Provider value={store}>{children}</StateContext.Provider>);
}
