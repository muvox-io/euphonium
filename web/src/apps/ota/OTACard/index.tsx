import { useEffect, useState } from "preact/hooks";
import { eventSource, getOTAManifest, triggerOTA } from "../../../api/euphonium/api";
import {OTAManifest} from "../../../api/euphonium/models";
import Button from "../../../components/ui/Button";
import Card from "../../../components/ui/Card";
import SelectItem from "../../../components/ui/SelectItem";

const UpdateRequested = ({ onClick = () => {} }) => {
    const [otaManifest, setOtaManifest] = useState<OTAManifest>(null as any);
    useEffect(() => {
        getOTAManifest().then(setOtaManifest);
    }, []);

  return (
    <div class="flex flex-col space-y-3">
      <div class="text-app-text-secondary text-s flex-row flex">
        requested update
      </div>
      <SelectItem>
        <div class="text-lg">New firmware</div>
        <div class="text-m text-app-text-secondary">
          {otaManifest?.name || '' }
        </div>
        <div class="text-xs text-app-text-secondary">
          {otaManifest?.sha256 || ''} 
        </div>
      </SelectItem>
      <div class="pt-1">
        <Button onClick={() => { triggerOTA(); onClick(); }} type="primary">
          Flash update
        </Button>
      </div>
    </div>
  );
};

const UpdateInProgress = ({ progress = 0, total = 0, state = "downloading" }) => {
  const totalMB = Math.round(total / (100 * 1024)) / 10;
  const progressMB = Math.round(progress / (100 * 1024)) / 10;

  let subtext = `Downloaded ${progressMB}MB of ${totalMB}MB`;

  if (state == "flashing") {
    subtext = "Flashing downloaded update...";
  }

  let progressTotal = Math.round((progress / total) * 100);
  if (progressTotal > 95) {
    progressTotal = 95;
  }

  return (
    <div class="flex flex-col space-y-3">
      <div class="text-xl flex-row flex">update in progress</div>
      <div>
        { state == "flashing" ? <div class="text-m text-green-500">Update hash valid</div> : null }
        <div class="text-m text-app-text-secondary">
          {subtext}
        </div>
      </div>

      <div class="pt-1">
        <Button progress={progressTotal} onClick={() => {}} type="progress">
          Installing update...
        </Button>
      </div>
    </div>
  );
};

const UpdateResult = ({state=""}) => {
  return (
    <div class="flex flex-col space-y-3">
      <div class="text-xl flex-row flex">{state == "finished" ? "update successful" : "update failed" }</div>
      <div>
        { state == "invalid_sha" ? <div class="text-m text-red-500">Update hash invalid</div> : null }
        { state == "finished" ? <div class="text-m text-green-500">OTA finished, your device will now reboot.</div> : null }
        { state != "finished" ? <div class="text-m text-red-500">OTA failed, your device will now reboot.</div> : null }

      </div>

      <div class="pt-1">
        <Button onClick={() => {}} type="primary">
          Refresh page
        </Button>
      </div>
    </div>
  );
};

export default function ({}) {
  const [otaState, setOtaState] = useState<string>("waiting");
  const [otaProgress, setOtaProgress] = useState<{
    total: number;
    progress: number;
  }>({ total: 0, progress: 0 });

  useEffect(() => {
    eventSource.addEventListener("ota_progress", ({ data }: any) => {
      let event = JSON.parse(data);
      setOtaProgress(event);
      setOtaState("downloading");
    });

    eventSource.addEventListener("ota_state", ({ data }: any) => {
      let { status } = JSON.parse(data);
      setOtaState(status);
    });
  }, []);
  return (
    <div class="md:w-[600px] md:absolute md:left-1/2 md:transform md:-translate-x-1/2 w-full">
      <Card
        enableButton={false}
        title="Recovery mode"
        subtitle="firmware update"
      >
        <>
          {otaState == "waiting" ? <UpdateRequested onClick={() => setOtaState("downloading") } /> : null}
          {otaState == "finished"  || otaState == 'invalid_sha' || otaState == 'failed' ? <UpdateResult state={otaState}/> : null }
          {otaState == "downloading" || otaState == "flashing" ? (
            <UpdateInProgress {...otaProgress} state={otaState} />
          ) : null}
        </>
      </Card>
    </div>
  );
}
