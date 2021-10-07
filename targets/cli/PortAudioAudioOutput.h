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
    void update(std::shared_ptr<CircularBuffer> audioBuffer);

private:
    PaStream *stream;
};

#endif