#ifndef BUFFEREDAUDIOSINK_H
#define BUFFEREDAUDIOSINK_H

#include <vector>
#include <iostream>
#include "AudioSink.h"
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"

class BufferedAudioSink : public AudioSink
{
public:
    void feedPCMFrames(const uint8_t *buffer, size_t bytes);
	bool setRate(uint16_t sampleRate) override;
protected:
    void startI2sFeed(size_t buf_size = 4096 * 8);
    void feedPCMFramesInternal(const void *pvItem, size_t xItemSize);
private:
};

#endif