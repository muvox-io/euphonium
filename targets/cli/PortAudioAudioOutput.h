#ifndef PORTAUDIO_OUTPUT_H
#define PORTAUDIO_OUTPUT_H
#include <AudioOutput.h>
#include <vector>
#include <iostream>
#include "portaudio.h"
#include <stdint.h>
class PortAudioAudioOutput : public AudioOutput
{
public:
    PortAudioAudioOutput();
    ~PortAudioAudioOutput();
    void setupBindings(std::shared_ptr<berry::VmState>) {};
    void feedPCMFrames(uint8_t* data, size_t nBytes);

private:
    PaStream *stream;
};

#endif
