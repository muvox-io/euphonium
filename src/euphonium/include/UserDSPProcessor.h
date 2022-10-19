#ifndef EUPHOINIUM_USERDSPPROCESSOR_H
#define EUPHOINIUM_USERDSPPROCESSOR_H

#include "AudioProcessors.h"
#include "BerryBind.h"
#include "BiquadFilter.h"
#include <cmath>
#include <memory>
#include <vector>

#define BIQUAD_TYPE_HIGHPASS 0
#define BIQUAD_TYPE_LOWPASS 1
#define BIQUAD_TYPE_NOTCH 2
#define BIQUAD_TYPE_PEAK 3
#define BIQUAD_TYPE_LOWSHELF 4
#define BIQUAD_TYPE_HIGHSHELF 5
#define BIQUAD_TYPE_PEAKING_EQ 6

class UserDSPProcessor : public AudioProcessor {
  private:
    std::vector<std::unique_ptr<BiquadFilter>> leftChannelBiquads;
    std::vector<std::unique_ptr<BiquadFilter>> rightChannelBiquads;

    bool enableDownmix = false;
    std::mutex reconfigureMutex;
    float currentGain = 1;

  public:
    UserDSPProcessor(){};

    ~UserDSPProcessor(){

    };

    void process(float *dataLeft, float *dataRight, size_t samplesPerChannel) {
        std::lock_guard<std::mutex> lock(reconfigureMutex);

        if (currentGain != 1) {
            // Apply gain
            for (size_t i = 0; i < samplesPerChannel; i++) {
                dataLeft[i] *= currentGain;
                dataRight[i] *= currentGain;
            }
        }

        if (enableDownmix) {
            for (size_t i = 0; i < samplesPerChannel; i++) {
                dataLeft[i] = (dataLeft[i] + dataRight[i]) / 2;
                dataRight[i] = dataLeft[i];
            }
        }

        for (size_t i = 0; i < rightChannelBiquads.size(); i++) {
            // process the right channel
            rightChannelBiquads[i]->processSamples(dataRight,
                                                   samplesPerChannel);
        }

        for (size_t i = 0; i < leftChannelBiquads.size(); i++) {
            // process the left channel
            leftChannelBiquads[i]->processSamples(dataLeft, samplesPerChannel);
        }
    }

    void setDownmix(bool downmix) {
        std::lock_guard<std::mutex> lock(reconfigureMutex);
        enableDownmix = downmix;
    }

    void registerBiquad(int channel, int type, float f, float g, float q) {
        std::lock_guard<std::mutex> lock(reconfigureMutex);

        auto biquad = std::make_unique<BiquadFilter>();

        switch (type) {
        case BIQUAD_TYPE_HIGHPASS:
            biquad->generateHighPassCoEffs(f, q);
            break;
        case BIQUAD_TYPE_LOWPASS:
            biquad->generateLowPassCoEffs(f, q);
            break;
        case BIQUAD_TYPE_NOTCH:
            biquad->generateNotchCoEffs(f, g, q);
            break;
        case BIQUAD_TYPE_PEAK:
            biquad->generatePeakCoEffs(f, g, q);
            break;
        case BIQUAD_TYPE_LOWSHELF:
            biquad->generateLowShelfCoEffs(f, g, q);
            break;
        case BIQUAD_TYPE_HIGHSHELF:
            biquad->generateHighShelfCoEffs(f, g, q);
            break;
        case BIQUAD_TYPE_PEAKING_EQ:
            biquad->generatePeakingEqCoEffs(f, g, q);
            break;
        }

        if (channel == 0) {
            leftChannelBiquads.push_back(std::move(biquad));
        } else {
            rightChannelBiquads.push_back(std::move(biquad));
        }
    }

    void setGainDb(float gainDb) {
        std::lock_guard<std::mutex> lock(reconfigureMutex);
        currentGain = pow(10, gainDb / 20);
    }

    void clearChain() {
        std::lock_guard<std::mutex> lock(reconfigureMutex);
        leftChannelBiquads.clear();
        rightChannelBiquads.clear();
        enableDownmix = false;
    }

    void setBindings(std::shared_ptr<berry::VmState> berry) {
        berry->export_this("set_gain_db", this, &UserDSPProcessor::setGainDb,
                           "dsp");
        berry->export_this("set_mono_downmix", this,
                           &UserDSPProcessor::setDownmix, "dsp");
        berry->export_this("register_biquad", this,
                           &UserDSPProcessor::registerBiquad, "dsp");
        berry->export_this("clear_chain", this, &UserDSPProcessor::clearChain,
                           "dsp");
    }
};

#endif
