#ifndef EUPHONIUM_AUDIO_PROCCESSORS_H
#define EUPHONIUM_AUDIO_PROCCESSORS_H

#include <memory>

class AudioProcessor
{
public:
    AudioProcessor() {};
    virtual ~AudioProcessor() {};
    virtual void process(uint8_t* data, size_t nBytes) = 0;
    virtual void setBindings(std::shared_ptr<berry::VmState>) = 0;
};

class AudioProcessors {
private:
    std::vector<std::unique_ptr<AudioProcessor>> processors;
    bool signedness;

public:
    AudioProcessors() {};
    ~AudioProcessors() {};

    void setSignedness(bool value){
        EUPH_LOG(info, "buf", "Setting signedness %s", value ? "true" : "false");
        signedness = value;
    }

    void setBindings(std::shared_ptr<berry::VmState> L) {
        for (auto& p : processors) {
            p->setBindings(L);
        }
        L->export_this("setSignedness", this, &AudioProcessors::setSignedness);
    }

    void addProcessor(std::unique_ptr<AudioProcessor> processor) {
        processors.push_back(std::move(processor));
    }
    void process(uint8_t* data, size_t nBytes) {
        for (auto& processor : processors) {
            processor->process(data, nBytes);
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
