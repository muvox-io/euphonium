import { useState } from 'preact/hooks';
import { Connectivity } from '../../api/euphonium/system/models';
import Button from '../ui/Button';
import WiFiConfigurator from '../WiFiConfig';

const FloatingCard = ({ children, header, className }: any) => {
  return (<div class="bg-app-primary h-screen lg:h-auto text-3xl text-app-text-primary pr-8 pt-8 pb-6 pl-8 lg:rounded-[40px] m-auto flex flex-col">
    {header}
    {children}
  </div>)
}

const OnboardingHello = ({ setOnboardingState }: any) => {
  return (<FloatingCard header="Hello!">
    <div class="text-[16px] text-app-text-secondary mt-4 mb-20 lg:w-[500px] leading-6">
      Thanks for trying out Euphonium. This wizard will guide you
      through the initial setup of your device.
      <br /><br />
      Before we begin, make sure you have access to a stable network, and know it’s credentials.
    </div>

    <Button onClick={() => {
      setOnboardingState('WIFI')
    }} class="primary text-xl h-[50px]">Get started</Button>
  </FloatingCard>)
};

const OnboardingHardwareDetected = ({ setOnboardingState }: any) => {
  return (<FloatingCard header="Detected official hardware">
    <div class="bg-app-secondary rounded-xl lg:w-[450px] flex flex-col justify-center items-center mt-5 mb-5">
      <img class="w-[220px] mt-12 align-center" src="https://raw.githubusercontent.com/muvox-io/euphonium/f9676280584a502628a40afb9dca9f1a92297c36/docs/assets/boards/icon-muvox.svg"/>
      <div class="mt-5">μVox amplifier</div>
      <div class="text-xl text-app-text-secondary mb-6">revision 0.9</div>
      
    </div>

    <Button onClick={() => {
      setOnboardingState('WIFI')
    }} class="primary text-xl h-[50px]">Confirm</Button>
  </FloatingCard>)
};



interface OnboardingParams {
  connectivity: Connectivity
}

export default ({connectivity}: OnboardingParams) => {
  const [onboardingState, setOnboardingState] = useState<'HELLO' | 'WIFI'>('HELLO');

  return <div class="flex justify-center h-screen">{
    connectivity.state != "CONNECTED" ? (onboardingState == 'HELLO' ? <OnboardingHello setOnboardingState={setOnboardingState}/> : <WiFiConfigurator/>) : null
  }</div>
};
