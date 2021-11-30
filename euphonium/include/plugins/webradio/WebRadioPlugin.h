#ifndef EUPHONIUM_WEBRADIO_PLUGIN_H
#define EUPHONIUM_WEBRADIO_PLUGIN_H

#include "ScriptLoader.h"
#include "EuphoniumLog.h"
#include "Logger.h"
#include "CoreEvents.h"
#include "Module.h"
#include <atomic>
#include <mutex>
#include "Queue.h"
#include "Task.h"
#include "HTTPAudioStream.h"

class WebRadioPlugin : public bell::Task, public Module
{
private:
    std::shared_ptr<HTTPAudioStream> audioStream;
    std::atomic<bool> isRunning;
    std::mutex runningMutex;
    bell::Queue<std::pair<bool, std::string>> radioUrlQueue;

public:
    WebRadioPlugin();
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader);
    void setupBindings();
    void shutdown();
    void configurationUpdated();
    void startAudioThread();
    void runTask();
    void playRadioUrl(std::string, bool);
};

#endif
