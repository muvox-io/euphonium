#ifndef DAC_OUTPUT_H
#define DAC_OUTPUT_H
#include <AudioOutput.h>
#include <vector>
#include <iostream>
#include <stdint.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <mutex>
#include <atomic>
#include "esp_err.h"
#include "esp_log.h"
#include "BellUtils.h"
#include "driver/i2s.h"

class DACAudioOutput : public AudioOutput
{
private:
    std::mutex readingMutex;
    std::atomic<bool> isReading;
public:
    DACAudioOutput();
    ~DACAudioOutput();
    void feedPCMFrames(uint8_t* data, size_t nBytes);
    void setupBindings(std::shared_ptr<berry::VmState>);
    void setReadable(bool);
};

#endif
