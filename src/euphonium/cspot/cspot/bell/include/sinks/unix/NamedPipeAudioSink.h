#pragma once

#include <vector>
#include <fstream>
#include "AudioSink.h"

class NamedPipeAudioSink : public AudioSink
{
public:
    NamedPipeAudioSink();
    ~NamedPipeAudioSink();
    void feedPCMFrames(const uint8_t *buffer, size_t bytes);
    
private:
    std::ofstream namedPipeFile;
};
