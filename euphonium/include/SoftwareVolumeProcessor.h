#ifndef EUPHONIUM_SOFTWARE_VOLUME_PROCESSOR_H
#define EUPHONIUM_SOFTWARE_VOLUME_PROCESSOR_H

#include "AudioProcessors.h"
#include <cmath>
#define VOL_MAX 100

class SoftwareVolumeProcessor : public AudioProcessor
{
private:
    uint32_t logVolume;
    bool signedness;

public:
    SoftwareVolumeProcessor() {
        setVolume(VOL_MAX / 2);
        setSignedness(false);
    };
    ~SoftwareVolumeProcessor() {};

    void setSignedness(bool value){
        signedness = value;
    }

    void setVolume(int volume)
    {
        EUPH_LOG(info, "buf", "Setting volume %d", volume);
        auto newVol = (volume / (double)VOL_MAX) * 255;

        // Calculate and cache log volume value
        auto vol = 255 - newVol;
        uint32_t value = (log10(255 / ((float)vol + 1)) * 105.54571334);
        if (value >= 254)
            value = 256;
        logVolume = value << 8; // *256
    }

    void process(uint8_t* data, size_t nBytes)
    {
        int16_t *psample;
        uint32_t pmax;
        psample = (int16_t *)(data);
        for (int32_t i = 0; i < (nBytes / 2); i++)
        {
            int32_t temp;
            // Offset data for unsigned sinks
            if(signedness) {
                temp = ((int32_t)psample[i] + 0x8000) * logVolume;
            } else {
                temp = ((int32_t)psample[i]) * logVolume;
            }
            psample[i] = (temp >> 16) & 0xFFFF;
        }
    }

    void setBindings(std::shared_ptr<berry::VmState> berry)
    {
        berry->export_this("setVolume", this, &SoftwareVolumeProcessor::setVolume);
        berry->export_this("setSignedness", this, &SoftwareVolumeProcessor::setSignedness);
    }
};

#endif
