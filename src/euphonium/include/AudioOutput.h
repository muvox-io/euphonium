#ifndef EUPHONIUM_AUDIO_OUTPUT_H
#define EUPHONIUM_AUDIO_OUTPUT_H

#include "BerryBind.h"
#include "CircularBuffer.h"
#include <memory>

class AudioOutput {
  public:
    enum class SampleFormat { INT16, INT32 };

    AudioOutput() {}
    virtual ~AudioOutput() {}

    /**
     * Called during script bindings initialization stage. All required bindings should
     * be added in this call
     * @param vm shared_ptr to berry vm
     */
    virtual void setupBindings(std::shared_ptr<berry::VmState> vm) = 0;

    /**
     * Feeds raw pcm data into given audio output
     * @param data Pointer to buffer containing raw pcm data
     * @param nBytes number of bytes to read
     */
    virtual void feedPCMFrames(uint8_t *data, size_t nBytes) = 0;

    /**
     * Called before incoming data changes formats
     * @param format PCM data format
     * @param sampleRate Requested sample rate
     */
    virtual void configureOutput(SampleFormat format, int sampleRate) = 0;
};

#endif
