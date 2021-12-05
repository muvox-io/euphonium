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

#define MAX_INT16 32767

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

    std::vector<float> dataLeft = std::vector<float>(1024);
    std::vector<float> dataRight = std::vector<float>(1024);

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
        lowShelfLeft->generateLowShelfCoEffs(EQUALIZER_LOW_SHELF_FREQUENCY / SAMPLE_FREQUENCY, (low - 2) * EQUALIZER_GAIN, Q_FACTOR);
        lowShelfRight->generateLowShelfCoEffs(EQUALIZER_LOW_SHELF_FREQUENCY / SAMPLE_FREQUENCY, (low - 2) * EQUALIZER_GAIN, Q_FACTOR);

        notchLeft->generateNotchCoEffs(EQUALIZER_NOTCH_FREQUENCY / SAMPLE_FREQUENCY, (mid - 2) * EQUALIZER_GAIN, Q_FACTOR / 2);
        notchRight->generateNotchCoEffs(EQUALIZER_NOTCH_FREQUENCY / SAMPLE_FREQUENCY, (mid - 2) * EQUALIZER_GAIN, Q_FACTOR / 2);

        highShelfLeft->generateHighShelfCoEffs(EQUALIZER_HIGH_SHELF_FREQUENCY / SAMPLE_FREQUENCY, (high - 2) * EQUALIZER_GAIN, Q_FACTOR);
        highShelfRight->generateHighShelfCoEffs(EQUALIZER_HIGH_SHELF_FREQUENCY / SAMPLE_FREQUENCY, (high - 2) * EQUALIZER_GAIN, Q_FACTOR);

        EUPH_LOG(info, "eq", "setBands: low: %.2f, mid: %.2f, high: %.2f", low, mid, high);
    }

    void process(uint8_t *data, size_t nBytes)
    {

        int16_t *data_16 = (int16_t *)data;

        int channel_length_16 = nBytes / 4;

        for (size_t i = 0; i < channel_length_16; i++)
        {
            dataLeft[i] = data_16[i * 2] / (float) MAX_INT16;      // Normalize left
            dataRight[i] = data_16[i * 2 + 1] / (float) MAX_INT16; // Normalize right
        }

        lowShelfLeft->processSamples(dataLeft.data(), channel_length_16);
        lowShelfRight->processSamples(dataRight.data(), channel_length_16);

        notchLeft->processSamples(dataLeft.data(), channel_length_16);
        notchRight->processSamples(dataRight.data(), channel_length_16);

        highShelfLeft->processSamples(dataLeft.data(), channel_length_16);
        highShelfRight->processSamples(dataRight.data(), channel_length_16);

        for (size_t i = 0; i < channel_length_16; i++)
        {
            data_16[i * 2] = dataLeft[i] * MAX_INT16;     // Denormalize left
            data_16[i * 2 + 1] = dataRight[i] * MAX_INT16; // Denormalize right
        }
    }

    void setBindings(std::shared_ptr<berry::VmState> berry)
    {
        berry->export_this("eqSetBands", this, &EqualizerProcessor::setBands);
    }
};

#endif
