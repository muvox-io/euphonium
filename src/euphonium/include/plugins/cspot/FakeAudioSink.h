#ifndef EUPHONIUM_CSPOT_AUDIO_SINK_H
#define EUPHONIUM_CSPOT_AUDIO_SINK_H

#include "AudioSink.h"
#include "MainAudioBuffer.h"
#include <memory>
#include "EuphoniumLog.h"
#include "EventBus.h"
#include "CoreEvents.h"
#include "ConstantParameters.h"

class FakeAudioSink: public AudioSink {
    public:
    std::shared_ptr<MainAudioBuffer> buffer;
    std::shared_ptr<EventBus> eventBus;
    FakeAudioSink(std::shared_ptr<MainAudioBuffer> buffer, std::shared_ptr<EventBus> eventBus);
    void feedPCMFrames(const uint8_t *buffer, size_t bytes);
    void volumeChanged(uint16_t volume);
};

#endif