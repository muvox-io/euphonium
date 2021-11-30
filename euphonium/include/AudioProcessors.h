#ifndef EUPHONIUM_AUDIO_PROCCESSORS_H
#define EUPHONIUM_AUDIO_PROCCESSORS_H

#include <memory>

class AudioProcessor
{
public:
    AudioProcessor() {};
    virtual ~AudioProcessor() {};
    virtual void process(uint8_t* data, size_t nBytes) = 0;
    virtual void setBindings(std::shared_ptr<Berry>) = 0;
};

class AudioProcessors {
private:
    std::vector<std::unique_ptr<AudioProcessor>> processors;

public:
    AudioProcessors() {};
    ~AudioProcessors() {};

    void setBindings(std::shared_ptr<Berry> L) {
        for (auto& p : processors) {
            p->setBindings(L);
        }
    }

    void addProcessor(std::unique_ptr<AudioProcessor> processor) {
        processors.push_back(std::move(processor));
    }
    void process(uint8_t* data, size_t nBytes) {
        for (auto& processor : processors) {
            processor->process(data, nBytes);
        }
    }
};

#endif
