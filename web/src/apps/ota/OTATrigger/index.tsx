import Input from "../../../components/ui/Input";
import Button from "../../../components/ui/Button";
import { useEffect, useState } from "preact/hooks";
import { getInfo, setOTAManifest } from "../../../api/euphonium/api";
import SelectItem from "../../../components/ui/SelectItem";
import useAPI from "../../../utils/useAPI.hook";
import OtaAPI from "../../../api/euphonium/ota/OtaAPI";

export default function ({ }) {
    const otaAPI = useAPI(OtaAPI);
    const [otaUrl, setOtaUrl] = useState<string>("");
    const [sha256, setSha256] = useState<string>("");
    const [newVersion, setNewVersion] = useState<string>("");

    useEffect(() => {
        fetch("https://api.github.com/repos/feelfreelinux/euphonium/releases/latest")
            .then((e) => e.json())
            .then(async ({ tag_name, assets, body }) => {
                const info = await otaAPI.getInfo();
                const otaBin = assets.find((asset: any) => asset.name == "ota.bin")
                const checkSumIndex = body.indexOf("ota_checksum: ") + 14
                const sha256 = body.substring(checkSumIndex, checkSumIndex + 64);
                if (tag_name != info.version) {
                    setOtaUrl(otaBin.browser_download_url)
                    setSha256(sha256);
                    setNewVersion(tag_name);
                }

            })
    }, []);
    if (newVersion != "") {
        return (<>
            <div class="text-green-600">New version {newVersion} available on github.</div>
            <Button disabled={false} type="primary" onClick={() => {
                setOTAManifest({ url: otaUrl, sha256: sha256 });
            }}>Flash update</Button>
        </>)
    } else {
        return (<div>Already on newest version</div>);
    }
    return (<>
        <Input value={otaUrl} onChange={setOtaUrl} tooltip="OTA update url" />
        <Input tooltip="sha256" value={sha256} onChange={setSha256} />
        <div class="pt-2 w-full">
            <Button disabled={!otaUrl.length || !sha256.length} type="primary" onClick={() => {
                otaAPI.setOTAManifest({ url: otaUrl, sha256: sha256 });
            }}>Reboot to bootloader</Button>
        </div>
    </>);
}
