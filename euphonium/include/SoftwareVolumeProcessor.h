#ifndef EUPHONIUM_SOFTWARE_VOLUME_PROCESSOR_H
#define EUPHONIUM_SOFTWARE_VOLUME_PROCESSOR_H

#include "AudioProcessors.h"
#include <cmath>
#define VOL_MAX 100

class SoftwareVolumeProcessor : public AudioProcessor {
  private:
    uint32_t logVolume;
    std::vector<uint8_t> volumeLookup = std::vector<uint8_t>(
        {255, 160, 120, 100, 90, 85, 80, 75, 70, 65, 61, 57, 53, 50, 47, 44, 41,
         38,  35,  32,  29,  26, 23, 20, 17, 14, 12, 10, 8,  6,  4,  2,  0});

  public:
    SoftwareVolumeProcessor() { setVolume(70); };
    ~SoftwareVolumeProcessor(){};

    void setVolume(int volume) {
        EUPH_LOG(info, "buf", "Setting volume %d", volume);
        auto newVol = ((100-volume) / (double)VOL_MAX) * 32;

        uint8_t value = (uint8_t)volumeLookup[newVol];
        logVolume = value << 8; // *256
    }

    void process(uint8_t *data, size_t nBytes) {
        int16_t *psample;
        psample = (int16_t *)(data);
        for (int32_t i = 0; i < (nBytes / 2); i++) {
            int32_t temp = ((int32_t)psample[i]) * logVolume;
            psample[i] = ((temp >> 16) & 0xFFFF);
        }
    }

    void setBindings(std::shared_ptr<berry::VmState> berry) {
        berry->export_this("set_soft_volume", this,
                           &SoftwareVolumeProcessor::setVolume, "playback");
    }
};

#endif
