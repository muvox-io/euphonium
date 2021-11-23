#include "FakeAudioSink.h"

FakeAudioSink::FakeAudioSink(std::shared_ptr<MainAudioBuffer> buffer, std::shared_ptr<EventBus> eventBus)
{
    this->buffer = buffer;
    this->eventBus = eventBus;
    this->softwareVolumeControl = false;
}

void FakeAudioSink::volumeChanged(uint16_t volume)
{
    int newVol = (volume / (double)MAX_VOLUME) * 100;
    auto event = std::make_unique<VolumeChangedEvent>(newVol);
    EUPH_LOG(info, "cspot", "Volume changed");
    this->eventBus->postEvent(std::move(event));
}

void FakeAudioSink::feedPCMFrames(std::vector<uint8_t> &data)
{
    size_t bytesWritten = 0;
    while (bytesWritten < data.size())
    {
        bytesWritten += buffer->write(data.data() + bytesWritten, data.size() - bytesWritten);
    }

    //buffer->audioBufferSemaphore->give();
}
