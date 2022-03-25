import { useCallback, useEffect, useState } from "preact/hooks"
import { connectToWifi, eventSource, getWifiStatus, scanWifi } from "../../../api/euphonium/api"
import { WiFiNetwork, WiFiState } from "../../../api/euphonium/models"
import Button from "../../../components/ui/Button"
import Card from "../../../components/ui/Card"
import Input from "../../../components/ui/Input"
import SelectItem from "../../../components/ui/SelectItem"
import Spinner from "../../../components/Spinner"

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

const NetworkSelected = ({ network, onConnect, onCancel }: { network: WiFiNetwork, onConnect: (ssid: string, pass: string) => void, onCancel: () => void }) => {
    const [password, setPassword] = useState("")
    return (<div class="flex flex-col space-y-3">

        <div class="flex flex-col space-y-3">
            <div class='text-gray-400 text-s flex-row flex'>selected network</div>
            <SelectItem isSelected={true}> {network.ssid} </SelectItem>
            {network?.open ? <></> :
                <><div class='text-gray-400 text-s flex-row flex'>Enter credentials</div>
                    <input
                        placeholder="password"
                        className="bg-app-secondary h-[58px] border border-app-border p-3 rounded-xl min-w-full"
                        value={password}
                        type="password"
                        onInput={(e: any) => setPassword(e.target.value)}
                    ></input></>
            }
            <div class="pt-2"><Button onClick={() => {
                onConnect(network.ssid, password);
            }} type='primary'>Connect</Button>

            </div>
            <div class="pt-2"><Button onClick={() => {
                onCancel();
            }} type='danger'>Cancel</Button>

            </div>
        </div>

    </div>)
}

const NetworkScanResults = ({ wifiState, setLocalState, setSelectedNetwork }: { wifiState: WiFiState, setLocalState: any, setSelectedNetwork: any }) => {
    return (<div class="flex flex-col space-y-3">
        <p><div class='text-gray-400 text-s mb-1 flex-row flex'>available networks (<div class='ml-1 mr-1 text-green-400 text-s animate-pulse'>scanning</div>)</div></p>
        <div class="flex flex-col space-y-3 mt-2">
            {wifiState.ssids.map(network => (
                <SelectItem onClick={() => {
                    if (network.open) {
                        setLocalState(CurrentState.ReadyToConnect);
                    } else {
                        setLocalState(CurrentState.RequirePassword);
                    }
                    setSelectedNetwork(network);
                }}> {network.ssid} </SelectItem>))}

        </div>

    </div>)
}

export default () => {
    const [wifiState, setWifiState] = useState<WiFiState>({ ssids: [], state: '', ipAddress: '' });
    const [localState, setLocalState] = useState<CurrentState>(CurrentState.Scanning);
    const [selectedNetwork, setSelectedNetwork] = useState<WiFiNetwork | null>(null);

    const handleWiFiState = useCallback(({ data }: any) => {
        let state: WiFiState = JSON.parse(data)

        setWifiState({ ...state });

        if (wifiState.state == 'connecting') {
            setLocalState(CurrentState.Connecting);
        }

    }, []);

    const handleOnOnline = useCallback(() => {
        getWifiStatus().then(setWifiState);
    }, []);

    useEffect(() => {
        scanWifi();
        eventSource.addEventListener("wifi_state", handleWiFiState);
        window.addEventListener('ononline', handleOnOnline);
        return () => {
            eventSource.removeEventListener("wifi_state", handleWiFiState);
            window.removeEventListener("ononline", handleOnOnline);
        };
    }, [handleWiFiState]);

    const startConnectWifi = async (ssid: string, password: string) => {
        connectToWifi(selectedNetwork!!.ssid, password);
    }

    return (
        <div class="md:w-[600px] md:absolute md:left-1/2 md:transform md:-translate-x-1/2 w-full">
            <Card enableButton={false} title="WiFi configuration" subtitle="initial configuration">
                {!selectedNetwork && wifiState.state == 'scanning' ? (<NetworkScanResults wifiState={wifiState} setSelectedNetwork={setSelectedNetwork} setLocalState={setLocalState} />) : <></>}
                {selectedNetwork && wifiState.state == 'scanning' ? (<NetworkSelected
                    onCancel={() => setSelectedNetwork(null)}
                    onConnect={startConnectWifi}
                    network={selectedNetwork} />) : <></>}
                {wifiState.state == 'connecting' ? <p><div class='ml-1 mr-1 text-green-400 text-s animate-pulse'>Connecting to {selectedNetwork!!.ssid}</div></p> : null}
                {wifiState.state == 'connected' ? <ConnectedState ipAddr={wifiState.ipAddress} /> : null}
            </Card>
        </div>)
}
