import { useContext, useEffect, useState } from "preact/hooks";
import { setApiUrl } from "../../api/api";
import IDE from "../IDE";
import StateWrapper, { StateContext } from "../utils/state-context";
const App = ({}) => {
  const [ipAddr, setIpAddr] = useContext(StateContext).address;
  const setIpAddress = (newIp: string) => {
    setApiUrl("http://" + newIp);
    setIpAddr(newIp);
  }

  useEffect(() => {
    setIpAddress("192.168.1.234");
  }, []);

  const [curVal, setCurVal] = useState<string>("");
  if (ipAddr == "") {
    return (
      <div class="h-screen w-screen bg-app-primary flex justify-center items-center">
        <div class="flex flex-col items-center space-y-10">
          <div class="flex flex-col w-[420px] bg-app-secondary rounded-2xl text-white p-4">
            <span class="text-app-text-secondary">Provide euphonium's device ip address</span>
            <input onInput={({ target }: { target: any }) => setCurVal(target?.value)} class="bg-app-primary h-10 rounded-xl mt-3 p-2" placeholder="address" />
            {/* <span class="text-app-text-secondary mt-3">Instance history</span>
            <div class="bg-app-primary h-10 rounded-xl mt-3 p-2">192.168.1.108</div>*/}
            <button onClick={(e) => setIpAddress(curVal)} class="bg-green-600 mt-3 rounded-xl h-10">Connect</button> 
          </div>
        </div>
      </div>
    )
  } else {
    return <IDE/>
  }
}

export default () => {
  return (<StateWrapper>
    <App />
  </StateWrapper>)
}
