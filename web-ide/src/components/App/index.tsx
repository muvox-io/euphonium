import { useState } from "preact/hooks";
import IDE from "../IDE";
export default function () {
    const [ipAddr, setIpAddr] = useState<string>("");
    const [curVal, setCurVal] = useState<string>("");
  if (ipAddr == "") {
  return (
    <div class="h-screen w-screen bg-app-primary flex justify-center items-center">
        <div class="flex flex-col w-[420px] bg-app-secondary rounded-2xl text-white p-4">
            <span class="text-app-text-secondary">Provide euphonium's device ip address</span>
            <input onInput={({target}: {target: any}) => setCurVal(target?.value)} class="bg-app-primary h-10 rounded-xl mt-3 p-2" placeholder="address"/>
            <button onClick={(e) => setIpAddr(curVal)} class="bg-green-600 mt-3 rounded-xl h-10">Connect</button>
        </div>
    </div>
  )
  } else {
    return <IDE ipAddr={ipAddr}/>
  }
}
