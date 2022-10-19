#ifndef ES9018AUDIOSINK_H
#define ES9018AUDIOSINK_H

#include <vector>
#include <iostream>
#include "BufferedAudioSink.h"
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"

class ES9018AudioSink : public BufferedAudioSink
{
public:
    ES9018AudioSink();
    ~ES9018AudioSink();
private:
};

#endif