#ifndef PORTAUDIO_OUTPUT_H
#define PORTAUDIO_OUTPUT_H
#include <AudioOutput.h>
#include <vector>
#include <iostream>

class PortAudioAudioOutput : public AudioOutput
{
public:
    PortAudioAudioOutput();
    void update(std::shared_ptr<CircularBuffer> audioBuffer);

private:
};

#endif