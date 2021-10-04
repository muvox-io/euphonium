#ifndef EUPHONIUM_AUDIO_OUTPUT_H
#define EUPHONIUM_AUDIO_OUTPUT_H

#include <memory.h>
#include <CircularBuffer.h>

class AudioOutput
{
    public:
    AudioOutput() {}
    virtual ~AudioOutput() {}
    virtual void update(std::shared_ptr<CircularBuffer> audioBuffer) = 0;
};

#endif