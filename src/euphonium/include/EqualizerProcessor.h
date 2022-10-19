#ifndef EUPHOINIUM_EQUALIZERPROCESSOR_H
#define EUPHOINIUM_EQUALIZERPROCESSOR_H

#include "AudioProcessors.h"
#include <cmath>
#include "BiquadFilter.h"
#include <memory>
#include <vector>
#include "BerryBind.h"

#define SAMPLE_FREQUENCY 44100.0 // Hz

#define EQUALIZER_LOW_SHELF_FREQUENCY 250   // Hz
#define EQUALIZER_NOTCH_FREQUENCY 1000      // Hz
#define EQUALIZER_HIGH_SHELF_FREQUENCY 4000 // Hz

#define Q_FACTOR sqrt(2) / 2 // 0.7071067812
#define EQUALIZER_GAIN 3     // dB

class EqualizerProcessor : public AudioProcessor
{
private:
    uint32_t logVolume;

public:
    std::shared_ptr<BiquadFilter> lowShelfLeft;
    std::shared_ptr<BiquadFilter> lowShelfRight;
    std::shared_ptr<BiquadFilter> highShelfLeft;
    std::shared_ptr<BiquadFilter> highShelfRight;
    std::shared_ptr<BiquadFilter> notchLeft;
    std::shared_ptr<BiquadFilter> notchRight;

    EqualizerProcessor()
    {
        lowShelfLeft = std::make_shared<BiquadFilter>();
        lowShelfRight = std::make_shared<BiquadFilter>();
        highShelfLeft = std::make_shared<BiquadFilter>();
        highShelfRight = std::make_shared<BiquadFilter>();
        notchLeft = std::make_shared<BiquadFilter>();
        notchRight = std::make_shared<BiquadFilter>();

        setBands(0, 0, 0);
    };
    ~EqualizerProcessor(){};

    void setBands(float low, float mid, float high)
    {
        lowShelfLeft->generateLowShelfCoEffs(EQUALIZER_LOW_SHELF_FREQUENCY / SAMPLE_FREQUENCY, (low) * EQUALIZER_GAIN, Q_FACTOR);
        lowShelfRight->generateLowShelfCoEffs(EQUALIZER_LOW_SHELF_FREQUENCY / SAMPLE_FREQUENCY, (low) * EQUALIZER_GAIN, Q_FACTOR);

        notchLeft->generateNotchCoEffs(EQUALIZER_NOTCH_FREQUENCY / SAMPLE_FREQUENCY, (mid) * EQUALIZER_GAIN, Q_FACTOR / 2);
        notchRight->generateNotchCoEffs(EQUALIZER_NOTCH_FREQUENCY / SAMPLE_FREQUENCY, (mid) * EQUALIZER_GAIN, Q_FACTOR / 2);

        highShelfLeft->generateHighShelfCoEffs(EQUALIZER_HIGH_SHELF_FREQUENCY / SAMPLE_FREQUENCY, (high) * EQUALIZER_GAIN, Q_FACTOR);
        highShelfRight->generateHighShelfCoEffs(EQUALIZER_HIGH_SHELF_FREQUENCY / SAMPLE_FREQUENCY, (high) * EQUALIZER_GAIN, Q_FACTOR);

        EUPH_LOG(info, "eq", "setBands: low: %.2f, mid: %.2f, high: %.2f", low, mid, high);
    }

    void process(float* dataLeft, float* dataRight, size_t samplesPerChannel)
    {

        lowShelfLeft->processSamples(dataLeft, samplesPerChannel);
        lowShelfRight->processSamples(dataRight, samplesPerChannel);

        notchLeft->processSamples(dataLeft, samplesPerChannel);
        notchRight->processSamples(dataRight, samplesPerChannel);

        highShelfLeft->processSamples(dataLeft, samplesPerChannel);
        highShelfRight->processSamples(dataRight, samplesPerChannel);
    }

    void setBindings(std::shared_ptr<berry::VmState> berry)
    {
        berry->export_this("set_eq", this, &EqualizerProcessor::setBands, "playback");
    }
};

#endif
