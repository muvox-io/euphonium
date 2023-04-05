import { useEffect, useState } from "preact/hooks";
import PlaybackAPI from "../../../api/euphonium/playback/PlaybackAPI";
import {
  ConfigurationField,
  PluginConfiguration,
} from "../../../api/euphonium/plugins/models";
import PluginsAPI from "../../../api/euphonium/plugins/PluginsAPI";
import SystemAPI from "../../../api/euphonium/system/SystemAPI";
import useAPI from "../../../utils/useAPI.hook";
import usePlaybackState from "../../../utils/usePlaybackState.hook";
import Modal from "../../ui/Modal";
import Button from "../../ui/Button";

const HardwareDetectedModal = ({ configUpdated }: any) => {
  const pluginsAPI = useAPI(PluginsAPI);

  const confirmSetting = async () => {
    await pluginsAPI.updatePluginConfiguration(
      "general_settings",
      {
        onboardingHardware: "true",
      },
      false
    );
    configUpdated();
  };

  return (
    <Modal header="Detected official hardware">
      <div class="bg-app-secondary rounded-xl lg:w-[450px] flex flex-col justify-center items-center mt-5 mb-5">
        <img
          class="w-[220px] mt-12 align-center"
          src="https://raw.githubusercontent.com/muvox-io/euphonium/f9676280584a502628a40afb9dca9f1a92297c36/docs/assets/boards/icon-muvox.svg"
        />
        <div class="mt-5">μVox amplifier</div>
        <div class="text-xl text-app-text-secondary mb-6">revision 0.9</div>
      </div>

      <Button onClick={confirmSetting} class="primary text-xl h-[50px]">
        Confirm
      </Button>
    </Modal>
  );
};

const VolTuningButton = ({ isNegative, onTap, disabled }: any) => {
  return (
    <button
      onClick={() => {
        if (disabled) return;
        onTap();
      }}
      class={
        `w-[50px] h-[50px] rounded-xl flex flex-col items-center pt-[6px] text-[26px]` +
        (!disabled ? " hover:bg-app-secondary-dark bg-app-secondary" : "")
      }
    >
      {isNegative ? "-" : "+"}
    </button>
  );
};

const VolumeTuningModal = ({ configUpdated }: any) => {
  const playbackAPI = useAPI(PlaybackAPI);
  const systemAPI = useAPI(SystemAPI);
  const playbackState = usePlaybackState();

  const [volume, setVolume] = useState(0);
  const [isLoading, setIsLoading] = useState(false);

  useEffect(() => {
    if (
      !playbackState?.settings ||
      playbackState.settings?.state ||
      playbackState.settings.state == "queue_empty"
    ) {
      // TODO: replace with fs tune
      playbackAPI.updateVolume(0);
      playbackAPI.queryContextURI(
        "radio://https%3A%2F%2Fs2.radio.co%2Fs2b2b68744%2Flisten"
      );
    }
  }, []);
  const increaseVolume = () => {
    if (volume == 100) return;
    setVolume(volume + 1);
    playbackAPI.updateVolume(volume);
  };

  const decreaseVolume = () => {
    if (volume == 0) return;
    setVolume(volume - 1);
    playbackAPI.updateVolume(volume);
  };

  const confirmMaxVolume = async () => {
    try {
      setIsLoading(true);
      await systemAPI.setAudioHardwareMaxVolume(volume);
      configUpdated();
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <Modal header="Amplifier loudness tuning">
      <div class="text-[16px] text-app-text-secondary mt-4 mb-10 lg:w-[420px] leading-6">
        Keep increasing the volume until the tune being played is at the
        preferred maximum volume level. In case the sound becomes disorted,
        decrease it.
      </div>

      <div class="flex flex-row items-center mb-10 w-full">
        <VolTuningButton
          disabled={volume == 0}
          onTap={decreaseVolume}
          isNegative={true}
        />
        <div class="flex-grow flex flex-col justify-center items-center">
          <div class="text-[16px] text-app-text-secondary">Volume</div>
          <div class="text-[32px] text-app-text-primary">{volume}%</div>
        </div>
        <VolTuningButton
          disabled={volume == 100}
          onTap={increaseVolume}
          isNegative={false}
        />
      </div>
      <Button
        onClick={confirmMaxVolume}
        class="primary text-xl h-[50px]"
        loadingState={isLoading}
      >
        Confirm
      </Button>
    </Modal>
  );
};

export default ({}) => {
  return <></>;
};
