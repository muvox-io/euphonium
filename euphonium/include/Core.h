#ifndef EUPHONIUM_CORE_H
#define EUPHONIUM_CORE_H

#include <vector>
#include <memory>
#include <iostream>
#include "AudioOutput.h"

#define AUDIO_BUFFER_SIZE 4096 * 16

class Core {
private:
    std::shared_ptr<AudioOutput> currentOutput;

public:
    Core();
    void registerChildren();
    void selectAudioOutput(std::shared_ptr<AudioOutput> output);
    void logAvailableServices();
    void handleLuaThread();
    void handleServerThread();
    void handleAudioOutputThread();

    std::shared_ptr<CircularBuffer> audioBuffer;
};

#endif