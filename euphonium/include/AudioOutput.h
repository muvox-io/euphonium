#ifndef EUPHONIUM_AUDIO_OUTPUT_H
#define EUPHONIUM_AUDIO_OUTPUT_H

#include <memory>
#include "CircularBuffer.h"
#include "BerryBind.h"

class AudioOutput
{
    public:
    AudioOutput() {}
    virtual ~AudioOutput() {}
    virtual void setupBindings(std::shared_ptr<berry::VmState>) = 0;
    virtual void feedPCMFrames(uint8_t* data, size_t nBytes) = 0;
};

#endif
