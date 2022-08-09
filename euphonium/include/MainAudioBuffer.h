#ifndef EUPHONIUM_MAUDIOBUFFER_H
#define EUPHONIUM_MAUDIOBUFFER_H

// Definition of internal audio buffer size.
#define AUDIO_BUFFER_SIZE 4096 * 64

#include "CircularBuffer.h"
#include "EuphoniumLog.h"
#include "platform/WrappedSemaphore.h"
#include <cmath>
#include <memory>
#include <AudioOutput.h>
#include <BellUtils.h>
#include <atomic>

typedef std::function<void(std::string)> shutdownEventHandler;

/**
 * MainAudioBuffer is a wrapper exposed to plugins which handles audio transport.
 * Lacks logic, only passes the calls further down
 */
class MainAudioBuffer {
  private:
  public:

    std::shared_ptr<AudioOutput> audioOutput;
    std::shared_ptr<CircularBuffer> audioBuffer;
    std::mutex accessMutex;
    shutdownEventHandler shutdownListener;
    uint32_t sampleRate = 0;
    std::atomic<bool> isLocked = false;

    MainAudioBuffer() {
        audioBuffer = std::make_shared<CircularBuffer>(AUDIO_BUFFER_SIZE);
    }

    /**
     * Sends an event which reconfigures current audio output
     * @param format incoming sample format
     * @param sampleRate data's sample rate
     */
    void configureOutput(AudioOutput::SampleFormat format, uint32_t sampleRate) {
        if (this->sampleRate != sampleRate) {
            this->sampleRate = sampleRate;
            audioOutput->configureOutput(format, sampleRate);
        }
    }
    
    /**
     * Returns current sample rate
     * @return sample rate
     */
    uint32_t getSampleRate() {
        return sampleRate;
    }

    /**
     * Clears input buffer, to be called for track change and such
     */
    void clearBuffer() {
         /*audioBuffer->emptyBuffer();*/ 
        // Leave 0.5s space for fadeout
        audioBuffer->emptyExcept(this->sampleRate);
    }

    /**
     * Locks access to audio buffer. Call after starting playback
     */
    void lockAccess() {
        if (!isLocked) {
            clearBuffer();
            this->accessMutex.lock();
            isLocked = true;
        }
    }

    /**
     * Frees access to the audio buffer. Call during shutdown
     */
     void unlockAccess() {
        if (isLocked) {
            clearBuffer();
            this->accessMutex.unlock();
            isLocked = false;
        }
     }

    /**
     * Prompts every plugin except for one provided to shut down and release mutex
     * @param other name of plugin calling the function
     */
    void shutdownExcept(std::string other) {
        clearBuffer();
        this->shutdownListener(other);
    }

    /**
     * Write audio data to the main buffer
     * @param data pointer to raw PCM data
     * @param bytes number of bytes to be read from provided pointer
     * @return number of bytes read
     */
    size_t write(const uint8_t *data, size_t bytes) {
        size_t bytesWritten = 0;
        while (bytesWritten < bytes)
        {
            auto write = audioBuffer->write(data + bytesWritten, bytes - bytesWritten);
            bytesWritten += write;
            if (write == 0) {
                BELL_SLEEP_MS(10);
            }
        }

        return bytesWritten;
    }
};

#endif
