#ifndef EUPHONIUM_CSPOT_AUDIO_SINK_H
#define EUPHONIUM_CSPOT_AUDIO_SINK_H

#include "AudioSink.h"
#include "CircularBuffer.h"
#include <memory>

class FakeAudioSink: public AudioSink {
    public:
    std::shared_ptr<CircularBuffer> buffer;
    FakeAudioSink(std::shared_ptr<CircularBuffer> buffer);
    void feedPCMFrames(std::vector<uint8_t> &data);
};

#endif