#ifndef EUPHONIUM_AUDIO_PROCCESSORS_H
#define EUPHONIUM_AUDIO_PROCCESSORS_H

#include <memory>

#define MAX_INT16 32767

class AudioProcessor {
  public:
    AudioProcessor(){};
    virtual ~AudioProcessor(){};
    virtual void process(float *dataLeft, float *dataRight,
                         size_t samplesPerChannel) = 0;
    virtual void setBindings(std::shared_ptr<berry::VmState>) = 0;
};

class AudioProcessors {
  private:
    std::vector<std::unique_ptr<AudioProcessor>> processors;
    bool signedness = false;
    std::vector<float> dataLeft = std::vector<float>(1024);
    std::vector<float> dataRight = std::vector<float>(1024);

  public:
    AudioProcessors(){};
    ~AudioProcessors(){};

    void setSignedness(bool value) {
        EUPH_LOG(info, "buf", "Setting signedness %s",
                 value ? "true" : "false");
        signedness = value;
    }

    void setBindings(std::shared_ptr<berry::VmState> L) {
        for (auto &p : processors) {
            p->setBindings(L);
        }
        L->export_this("setSignedness", this, &AudioProcessors::setSignedness);
    }

    void addProcessor(std::unique_ptr<AudioProcessor> processor) {
        processors.push_back(std::move(processor));
    }
    void process(uint8_t *data, size_t nBytes, size_t bytesLeftInBuffer, size_t fadeoutBytes) {
        int16_t *data_16 = (int16_t *)data;

        int channel_length_16 = nBytes / 4;

        for (size_t i = 0; i < channel_length_16; i++) {
            dataLeft[i] = data_16[i * 2] / (float)MAX_INT16; // Normalize left
            dataRight[i] =
                data_16[i * 2 + 1] / (float)MAX_INT16; // Normalize right
        }

        for (auto &processor : processors) {
            processor->process(dataLeft.data(), dataRight.data(), nBytes / 4);
        }

        for (size_t i = 0; i < channel_length_16; i++) {
            size_t actualBytesInBuffer = (bytesLeftInBuffer + nBytes);
            actualBytesInBuffer -= (channel_length_16 * 4) - (i*4);


            data_16[i * 2] = dataLeft[i]  * MAX_INT16; // Denormalize left
            data_16[i * 2 + 1] = dataRight[i] * MAX_INT16; // Denormalize right
            if (actualBytesInBuffer < fadeoutBytes) {
                int steps = (actualBytesInBuffer * 33) / fadeoutBytes;
                data_16[i*2] *= (float) steps / 33;
                data_16[i*2 + 1] *= (float) steps / 33;
            }

        }

        if (signedness) {
            int16_t *psample;
            uint32_t pmax;
            psample = (int16_t *)(data);
            for (int32_t i = 0; i < (nBytes / 2); i++) {
                psample[i] += 0x8000;
            }
        }
    }
};

#endif
