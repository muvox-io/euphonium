#include "FakeAudioSink.h"
#include "BellUtils.h"

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

void FakeAudioSink::feedPCMFrames(const uint8_t *buf, size_t bytes)
{
    size_t bytesWritten = 0;
    while (bytesWritten < bytes)
    {
        auto write = buffer->write(buf + bytesWritten, bytes - bytesWritten);
        bytesWritten += write;
        if (write == 0) {
            BELL_SLEEP_MS(10);
        }
    }

    //buffer->audioBufferSemaphore->give();
}
