import { useEffect, useState } from "preact/hooks";
import { getDACPresets } from "../../api/euphonium/api";
import { DACPreset } from "../../api/euphonium/models";
import { updatePluginConfiguration } from "../../api/euphonium/plugins";
import Button from "../../components/Button";
import Input from "../../components/Input";
import Modal from "../../components/Modal";
import SelectItem from "../../components/SelectItem";

export default function ({ configurationUpdated = () => {} }) {
  const [dacs, setDacs] = useState<DACPreset[]>([]);
  const [isOpen, setOpen] = useState<boolean>(false);

  const [selectedDAC, setDac] = useState<DACPreset>();
  useEffect(() => {
    getDACPresets().then(setDacs);
  }, []);

  return (
    <>
      <div class="text-app-text-secondary text-xl mb-1">
        Automated configuration
      </div>
      <Button
        type="primary"
        onClick={() => {
          setOpen(true);
        }}
      >
        Select your board
      </Button>

      {isOpen ? (
        <Modal header="Select your board">
          <div class="space-y-4 pt-2">
            <Input
              onSubmit={() => {}}
              value=""
              placeholder="Find your board"
              icon="search"
            />
            {dacs.map((dac) => (
              <SelectItem
                isSelected={selectedDAC == dac}
                onClick={() => setDac(dac)}
              >
                <div class="text-xl mb-2">{dac.board}</div>
                <div class="text-app-text-secondary text-l">
                  DAC: {dac.dac}, bck: {dac.bck}, ws: {dac.ws}, mclk: {dac.mclk}
                  , data: {dac.data}, sda: {dac.sda}, scl: {dac.scl}
                </div>
              </SelectItem>
            ))}
            <Button
              type="primary"
              disabled={!selectedDAC}
              onClick={() => {
                let dacConfig = selectedDAC as any;
                delete dacConfig["board"];
                updatePluginConfiguration("dac", selectedDAC, false).then((e) => {
                  configurationUpdated();
                  setOpen(false);
                  setDac(null as any);
                  getDACPresets().then(setDacs);
                });
              }}
            >
              Confirm selection
            </Button>
          </div>
        </Modal>
      ) : null}

      <div class="text-app-text-secondary text-xl mb-1">
        Manual configuration
      </div>
    </>
  );
}
