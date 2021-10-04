#ifndef EUPHONIUM_CSPOT_AUDIO_SINK_H
#define EUPHONIUM_CSPOT_AUDIO_SINK_H

#include "AudioSink.h"

class FakeAudioSink: public AudioSink {
    public:
    FakeAudioSink();
    void feedPCMFrames(std::vector<uint8_t> &data);
};

#endif