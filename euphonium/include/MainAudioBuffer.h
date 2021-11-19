#ifndef EUPHONIUM_MAUDIOBUFFER_H
#define EUPHONIUM_MAUDIOBUFFER_H
#define AUDIO_BUFFER_SIZE 4096 * 10

#include <memory>
#include "CircularBuffer.h"
#include "platform/WrappedSemaphore.h"

class MainAudioBuffer {
    public:
    std::shared_ptr<CircularBuffer> audioBuffer;
    std::unique_ptr<WrappedSemaphore> audioBufferSemaphore;
    int sampleRate = 44100;
    MainAudioBuffer() {
        audioBuffer = std::make_shared<CircularBuffer>(AUDIO_BUFFER_SIZE);
        audioBufferSemaphore = std::make_unique<WrappedSemaphore>(200);
    }

};

#endif