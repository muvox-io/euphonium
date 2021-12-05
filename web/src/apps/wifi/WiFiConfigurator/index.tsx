import { useEffect, useState } from "preact/hooks"
import { connectToWifi, eventSource, getWifiStatus, scanWifi } from "../../../api/euphonium/api"
import { WiFiNetwork, WiFiState } from "../../../api/euphonium/models"
import Button from "../../../components/Button"
import Card from "../../../components/Card"
import Input from "../../../components/Input"
import SelectItem from "../../../components/SelectItem"

enum CurrentState {
    Scanning,
    Connecting,
    ReadyToConnect,
    RequirePassword
}

const ConnectedState = ({ ipAddr = "" }) => {
    return ((
        <div class="flex flex-col space-y-3">
            <div class='text-gray-400 text-s flex-row flex'>WiFi connected successfully</div>
            <div class="text-sm text-gray-400 pb-1">your address is <i class="text-green-400">{ipAddr}</i>. Please connect to your network and connect via this address.</div>
            <Button onClick={() => {
                window.location.href = "http://" + ipAddr + "/web";
            }} type='primary'>Navigate to device</Button></div>));
}

export default () => {
    const [wifiState, setWifiState] = useState<WiFiState>({ ssids: [], state: '', ipAddress: '' });
    const [localState, setLocalState] = useState<CurrentState>(CurrentState.Scanning);
    const [selectedNetwork, setSelectedNetwork] = useState<WiFiNetwork | null>(null);
    const [password, setPassword] = useState<string>('');

    useEffect(() => {
        scanWifi();
        eventSource.addEventListener("wifi_state", ({ data }: any) => {
            setWifiState(JSON.parse(data));

            if (wifiState.state == 'connecting') {
                setLocalState(CurrentState.Connecting);
            }
        });

        window.addEventListener('ononline', (ev) => {
            getWifiStatus().then(setWifiState);
        });
    }, []);

    const startConnectWifi = async () => {
        connectToWifi(selectedNetwork!!.ssid, password);
    }

    return (
        <div class="md:w-[600px] md:absolute md:left-1/2 md:transform md:-translate-x-1/2 w-full">
            <Card enableButton={false} title="WiFi configuration" subtitle="initial configuration">
                {wifiState.state == 'connected' ? <ConnectedState/> : (
                    <div class="flex flex-col space-y-3">
                        <p><div class='text-gray-400 text-s mb-1 flex-row flex'>available networks (<div class='ml-1 mr-1 text-green-400 text-s animate-pulse'>scanning</div>)</div>
                        </p>
                        <div class="flex flex-col space-y-3 mt-2">
                            {wifiState.ssids.map(network => (
                                <SelectItem isSelected={selectedNetwork?.ssid == network.ssid} onClick={() => {
                                    if (network.open) {
                                        setLocalState(CurrentState.ReadyToConnect);
                                    } else {
                                        setLocalState(CurrentState.RequirePassword);
                                    }
                                    setSelectedNetwork(network);
                                }}> {network.ssid} </SelectItem>))}
                        </div>

                        {localState == CurrentState.RequirePassword ? <Input type="password" value={password} tooltip="Please enter password" onChange={setPassword} placeholder="password"></Input> : null}
                        {wifiState.state == 'connecting' ? <div class='ml-1 mr-1 text-green-400 text-sm animate-pulse'>connecting to {selectedNetwork?.ssid}</div> : null}
                        {wifiState.state == 'error' ? <div class='ml-1 mr-1 text-red-400 text-sm'>cannot connect to network</div> : null}
                        <div class="pt-2"><Button onClick={() => {
                            startConnectWifi();
                            setWifiState({ ssids: [], state: '', ipAddress: '' });
                        }} disabled={localState != CurrentState.ReadyToConnect && localState != CurrentState.RequirePassword} type='primary'>Connect to WiFi</Button>
                        </div></div>)}
            </Card></div>)
}