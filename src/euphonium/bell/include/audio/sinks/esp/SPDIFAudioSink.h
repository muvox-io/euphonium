#ifndef SPDIFAUDIOSINK_H
#define SPDIFAUDIOSINK_H

#include <vector>
#include <iostream>
#include "BufferedAudioSink.h"
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"

class SPDIFAudioSink : public BufferedAudioSink
{
private:
  	uint8_t spdifPin;
public:
    explicit SPDIFAudioSink(uint8_t spdifPin);
    ~SPDIFAudioSink() override;
    void feedPCMFrames(const uint8_t *buffer, size_t bytes) override;
	bool setParams(uint32_t sampleRate, uint8_t channelCount, uint8_t bitDepth) override;
private:
};

#endif