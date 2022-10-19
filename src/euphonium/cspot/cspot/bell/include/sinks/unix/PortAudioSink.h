#pragma once

#include <vector>
#include "portaudio.h"
#include <stdint.h>
#include <iostream>
#include "AudioSink.h"

class PortAudioSink : public AudioSink
{
public:
    PortAudioSink();
    ~PortAudioSink();
    void feedPCMFrames(const uint8_t *buffer, size_t bytes);
	void initialize(uint16_t sampleRate);
	bool setRate(uint16_t sampleRate) override;
    
private:
    PaStream *stream;
};
