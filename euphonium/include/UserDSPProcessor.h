#ifndef EUPHOINIUM_USERDSPPROCESSOR_H
#define EUPHOINIUM_USERDSPPROCESSOR_H

#include "AudioProcessors.h"
#include <cmath>
#include "BiquadFilter.h"
#include <memory>
#include <vector>
#include "BerryBind.h"


class UserDSPProcessor : public AudioProcessor
{
private:
    std::vector<BiquadFilter> leftChannelBiquads;
    std::vector<BiquadFilter> rightChannelBiquads;

    bool enableDownmix = false;
    std::mutex reconfigureMutex;
public:


    UserDSPProcessor()
    {
    };

    ~UserDSPProcessor() {

    };


    void process(float* dataLeft, float* dataRight, size_t samplesPerChannel)
    {
        std::lock_guard<std::mutex> lock(reconfigureMutex);
        for (size_t i = 0; i < rightChannelBiquads.size(); i++)
        {
            // process the right channel
            rightChannelBiquads[i].processSamples(dataRight, samplesPerChannel);
        }

        for (size_t i = 0; i < leftChannelBiquads.size(); i++)
        {
            // process the left channel
            leftChannelBiquads[i].processSamples(dataLeft, samplesPerChannel);
        }

        if (enableDownmix)
        {
            for (size_t i = 0; i < samplesPerChannel; i++)
            {
                dataLeft[i] = (dataLeft[i] + dataRight[i]) / 2;
                dataRight[i] = dataLeft[i];
            }
        }
    }

    void setDownmix(bool downmix) {
        std::lock_guard<std::mutex> lock(reconfigureMutex);
        enableDownmix = downmix;
    }

    void registerBiquad(int channel, int type, int f, int g, int q) {
        std::lock_guard<std::mutex> lock(reconfigureMutex);

        if (channel == 0)
        {
            // leftChannelBiquads.push_back(BiquadFilter(type, f, g, q));
        }
        else
        {
            // rightChannelBiquads.push_back(BiquadFilter(type, f, g, q));
        }
    }

    void clearChain() {
        std::lock_guard<std::mutex> lock(reconfigureMutex);
        leftChannelBiquads.clear();
        rightChannelBiquads.clear();
        enableDownmix = false;
    }

    void setBindings(std::shared_ptr<berry::VmState> berry)
    {
        berry->export_this("set_mono_downmix", this, &UserDSPProcessor::setDownmix, "dsp");
        berry->export_this("register_biquad", this, &UserDSPProcessor::registerBiquad, "dsp");
        berry->export_this("clear_chain", this, &UserDSPProcessor::clearChain, "dsp");
    }
};

#endif
