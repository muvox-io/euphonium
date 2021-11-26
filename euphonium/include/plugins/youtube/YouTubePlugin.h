#ifndef EUPHONIUM_YOUTUBE_PLUGIN_H
#define EUPHONIUM_YOUTUBE_PLUGIN_H

#include "ScriptLoader.h"
#include <sol.hpp>
#include "EuphoniumLog.h"
#include "Logger.h"
#include "CoreEvents.h"
#include "Module.h"
#include <atomic>
#include <mutex>
#include "Queue.h"
#include "MpegDashDemuxer.h"
#include "Task.h"
#include "HTTPStream.h"
#include "aacdec.h"
#include "DecoderGlobals.h"

class YouTubePlugin : public bell::Task, public Module
{
private:
    std::atomic<bool> isRunning;
    std::mutex runningMutex;
    bell::Queue<std::string> ytUrlQueue;
    AACFrameInfo aacFrameInfo;
    std::vector<short> outputBuffer;
public:
    YouTubePlugin();
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader);
    void setupLuaBindings();
    void shutdown();
    void configurationUpdated();
    void startAudioThread();
    void runTask();
    void playYTUrl(std::string);
};

#endif