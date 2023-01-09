import { useState } from 'preact/hooks';
import eventSource from '../../api/euphonium/eventSource';
import { ConnectivityEvent, WiFiNetwork } from '../../api/euphonium/wifi/models';
import WiFiAPI from '../../api/euphonium/wifi/WiFiAPI';
import useAPI from '../../utils/useAPI.hook';
import TextField from '../FormGroup/fields/TextField';
import Button from '../ui/Button';
import Icon from '../ui/Icon';
import Input from '../ui/NewInput';

const FloatingCard = ({ children, header, className }: any) => {
  return (<div class="bg-app-primary text-3xl text-app-text-primary pr-8 pt-8 pb-6 pl-8 rounded-[40px] m-auto flex flex-col">
    {header}
    {children}
  </div>)
}

const renderHello = () => {
  return (<FloatingCard header="Hello!">
    <div class="text-[16px] text-app-text-secondary mt-4 mb-20 w-[500px] leading-6">
      Thanks for trying out Euphonium. This wizard will guide you
      through the initial setup of your device.
      <br /><br />
      Before we begin, make sure you have access to a stable network, and know it’s credentials.
    </div>

    <Button class="primary text-xl h-[50px]">Get started</Button>
  </FloatingCard>)
};

const NetworkItem = ({ name, isOpen, onClick }: any) => {
  return (<div onClick={onClick} class="flex flex-row items-center text-lg pr-2 pl-6 h-[55px] text-app-text-primary rounded-2xl mt-2 mb-2 bg-app-secondary hover:opacity-70">{name}
    {!isOpen ? <div class="ml-auto"><Icon name='info'></Icon></div> : null}
  </div>)
};

interface NetworkListParams {
  setConfigState: (state: any) => void;
  connectivity?: ConnectivityEvent;
}

const NetworkList = ({ setConfigState, connectivity }: NetworkListParams) => {
  const hasNetworks = connectivity?.body?.networks?.length;
  const wifiAPI = useAPI(WiFiAPI);
  const scanning = !!connectivity?.body?.scanning;

  const doScan = () => {
    wifiAPI.wifiScan();
  }

  if (!connectivity?.body?.scanning && !hasNetworks) {
    doScan();
  }

  return (<FloatingCard header="Configure WiFi">
    <div class="text-[16px] text-app-text-secondary mt-3 mb-2 w-[500px] leading-6 flex flex-col">
      {hasNetworks ? "Available networks" : null}

      {hasNetworks ? connectivity?.body?.networks?.map((network: any) => {
        return (<NetworkItem name={network.ssid} isOpen={network.open} onClick={() => {
          setConfigState({ network, screen: 'networkSelected' });
        }} />)
      }) : (<div class="self-center mt-20 mb-20 flex flex-col items-center">
        <div class="dot-elastic mb-3"></div>
        scanning
      </div>)}

      {hasNetworks ? <><div class="mb-5"></div><Button
        onClick={doScan}
        class="text-xl h-[55px] w-full flex flex-row justify-center items-center">
                {scanning ? (<div class="dot-elastic"></div>) : 'Scan again'}

      </Button></> : null}
      {hasNetworks ? (<div class="mt-2 w-full text-center">Show remaining</div>) : null}
    </div>
  </FloatingCard>)
};

interface NetworkSelectedParams {
  setConfigState: (state: any) => void;
  network?: WiFiNetwork;
  connectivity?: ConnectivityEvent;
}

const NetworkSelected = ({ setConfigState, network, connectivity }: NetworkSelectedParams) => {
  const [password, setPassword] = useState('');

  const api = useAPI(WiFiAPI);

  const connect = () => {
    api.connectToWifi(network?.ssid!!, password);
  }

  const connecting = connectivity?.state == 'CONNECTING';

  return (<FloatingCard header="Configure WiFi">
    <div class="text-[16px] text-app-text-secondary mt-3 mb-2 w-[500px] leading-6">
      Selected network
      <NetworkItem name={network?.ssid} isOpen={network?.open} />
      {network?.open ? null : <>
        Network password
        <Input type="password" class="mt-1 mb-2 text-2xl" height={55} onChange={setPassword} value={password}></Input></>}


      <div class={`${connectivity?.body?.error ? "visible" : "invisible"} text-red-500 text-l mb-4`}>Connection failed, recheck credentials</div>
      <Button disabled={!network?.open && password == ''}
        onClick={() => connect()}
        class="primary text-xl h-[55px] w-full flex flex-row justify-center items-center">
        {connecting ? (<div class="dot-elastic"></div>) : 'Connect'}
      </Button>
      <div onClick={() => {
        setConfigState({ screen: 'networkList' });
      }} class="mt-3 w-full text-center cursor-pointer">Go back</div>
    </div>
  </FloatingCard>)
};

interface NetworkConnectedParams {
  setConfigState: (state: any) => void;
  connectivity?: ConnectivityEvent;
}

const NetworkConnected = ({ connectivity }: NetworkConnectedParams) => {
  return (<FloatingCard header="Configure WiFi">
    <div class="text-[16px] text-app-text-secondary mt-3 mb-2 w-[500px] leading-6">
      Network connected successfully
      <div class="flex flex-row items-center text-lg pr-2 pl-6 h-[55px] text-app-text-primary rounded-2xl mt-2 mb-2 bg-app-secondary">
        {connectivity?.ip}
        <div class="ml-auto"><Icon name='info'></Icon></div>
      </div>

      <div class="mt-4 mb-4 text-sm text-green-300">Note: Your browser will now connect back into your previous network, and redirect to the device</div>
      <Button
        onClick={() => { }}
        class="primary text-xl h-[55px] w-full flex flex-row justify-center items-center">
        Redirect
      </Button>
    </div>
  </FloatingCard>)
};


const WiFiConfigurator = () => {
  const [connectivityEvent, setConnectivityEvent] = useState<ConnectivityEvent>({
  });
  eventSource.on('connectivity', setConnectivityEvent);


  const [configState, setConfigState] = useState({
    screen: 'networkList',
    network: undefined 
  });

  if (configState.screen == 'networkList') {
    return <NetworkList setConfigState={setConfigState} connectivity={connectivityEvent} />
  }

  if (connectivityEvent.state == 'CONNECTED') {
    return <NetworkConnected setConfigState={setConfigState} connectivity={connectivityEvent} />
  }

  if (configState.screen == 'networkSelected') {
    return <NetworkSelected setConfigState={setConfigState} network={configState?.network} connectivity={connectivityEvent} />
  }



  return <></>;
};

export default WiFiConfigurator;
