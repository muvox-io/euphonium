import Input from "../../../components/Input";
import Button from "../../../components/Button";
import {useState} from "preact/hooks";
import {setOTAManifest} from "../../../api/euphonium/api";

export default function({}) {
    const [otaUrl, setOtaUrl] = useState<string>("");
    const [sha256, setSha256] = useState<string>("");
    return(<>
        <Input value={otaUrl} onChange={setOtaUrl} tooltip="OTA update url"/>
        <Input tooltip="sha256" value={sha256} onChange={setSha256}/>
        <div class="pt-2 w-full">
            <Button disabled={ !otaUrl.length || !sha256.length } type="primary" onClick={() => {
               setOTAManifest({ url: otaUrl, sha256: sha256 }); 
            }}>Reboot to bootloader</Button>
        </div>
    </>);
}
