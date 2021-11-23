#ifndef EUPHONIUM_AUDIO_OUTPUT_H
#define EUPHONIUM_AUDIO_OUTPUT_H

#include <memory>
#include <CircularBuffer.h>

class AudioOutput
{
    public:
    AudioOutput() {}
    virtual ~AudioOutput() {}
    virtual void feedPCMFrames(uint8_t* data, size_t nBytes) = 0;
};

#endif