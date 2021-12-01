#ifndef AC101_OUTPUT_H
#define AC101_OUTPUT_H
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
#include "esp_err.h"
#include "esp_log.h"
#include "ac101.h"
#include "adac.h"

class AC101AudioOutput : public AudioOutput
{
public:
    AC101AudioOutput();
    ~AC101AudioOutput();
    uint8_t *audioBuffer;
    void feedPCMFrames(uint8_t* data, size_t nBytes);
private:
    adac_s *dac;
};

#endif