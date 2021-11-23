#ifndef EUPHONIUM_MAUDIOBUFFER_H
#define EUPHONIUM_MAUDIOBUFFER_H

#define AUDIO_BUFFER_SIZE 4096 * 64


#include <memory>
#include "CircularBuffer.h"
#include <cmath>
#include "EuphoniumLog.h"
#include "platform/WrappedSemaphore.h"

typedef std::function<void(std::string)> shutdownEventHandler;

class MainAudioBuffer
{
public:
    std::shared_ptr<CircularBuffer> audioBuffer;
    std::unique_ptr<WrappedSemaphore> audioBufferSemaphore;
    shutdownEventHandler shutdownListener;

    int sampleRate = 44100;
    uint32_t logVolume = 1;

    MainAudioBuffer()
    {
        audioBuffer = std::make_shared<CircularBuffer>(AUDIO_BUFFER_SIZE);
        audioBufferSemaphore = std::make_unique<WrappedSemaphore>(200);
    }

    void shutdownExcept(std::string other) {
        this->shutdownListener(other);
    }

    size_t write(const uint8_t *data, size_t bytes)
    {
        return audioBuffer->write(data, bytes);
    }
};

#endif