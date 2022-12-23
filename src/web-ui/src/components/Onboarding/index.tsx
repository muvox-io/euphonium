import { useState } from 'preact/hooks';
import { Connectivity } from '../../api/euphonium/system/models';
import Button from '../ui/Button';
import WiFiConfigurator from '../WiFiConfig';

const FloatingCard = ({ children, header, className }: any) => {
  return (<div class="bg-app-primary text-3xl text-app-text-primary pr-8 pt-8 pb-6 pl-8 rounded-[40px] m-auto flex flex-col">
    {header}
    {children}
  </div>)
}

const OnboardingHello = ({ setOnboardingState }: any) => {
  return (<FloatingCard header="Hello!">
    <div class="text-[16px] text-app-text-secondary mt-4 mb-20 w-[500px] leading-6">
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

interface OnboardingParams {
  connectivity: Connectivity
}

export default ({connectivity}: OnboardingParams) => {
  const [onboardingState, setOnboardingState] = useState<'HELLO' | 'WIFI'>('HELLO');

  return <div class="flex justify-center h-screen">{
    connectivity.state != "CONNECTED" ? (onboardingState == 'HELLO' ? <OnboardingHello setOnboardingState={setOnboardingState}/> : <WiFiConfigurator/>) : null
  }</div>
};
