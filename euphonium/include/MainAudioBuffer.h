#ifndef EUPHONIUM_MAUDIOBUFFER_H
#define EUPHONIUM_MAUDIOBUFFER_H

#define AUDIO_BUFFER_SIZE 4096 * 64
#define VOL_MAX 100

#include <memory>
#include "CircularBuffer.h"
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
        setVolume(50);
    }

    void setVolume(int volume)
    {
        auto newVol = (volume / (double)VOL_MAX) * 255;

        // Calculate and cache log volume value
        auto vol = 255 - newVol;
        uint32_t value = (log10(255 / ((float)vol + 1)) * 105.54571334);
        if (value >= 254)
            value = 256;
        logVolume = value << 8; // *256
    }

    void shutdownExcept(std::string other) {
        this->shutdownListener(other);
    }

    size_t write(const uint8_t *data, size_t bytes)
    {
        // int16_t *psample;
        // uint32_t pmax;
        // psample = (int16_t *)(data);
        // for (int32_t i = 0; i < (bytes / 2); i++)
        // {
        //     int32_t temp;
        //     // Offset data for unsigned sinks

        //     temp = ((int32_t)psample[i] + 0x8000) * logVolume;

        //     temp = ((int32_t)psample[i]) * logVolume;

        //     psample[i] = (temp >> 16) & 0xFFFF;
        // }
        return audioBuffer->write(data, bytes);
    }
};

#endif