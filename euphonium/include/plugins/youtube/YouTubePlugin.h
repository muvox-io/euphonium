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
#include "platform/TLSSocket.h"

// Could've done it with a JSON parser, but wanted to save overhead
#define POST_BODY_PART1 "{\"context\":{\"client\":{\"hl\":\"en\",\"clientName\":\"WEB\",\"clientVersion\":\"2.20210721.00.00\",\"mainAppWebInfo\":{\"graftUrl\": \"/watch?v="
#define POST_BODY_PART2 "\"}}},\"videoId\":\""
#define POST_BODY_PART3 "\"}"

#define YT_PLAYER_KEY "AIzaSyAO_FJ2SlqU8Q4STEHLGCilw_Y9_11qcW8"

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
    std::string getStreamForVideo(std::string videoUrl);
    void setupBindings();
    void shutdown();
    void configurationUpdated();
    void startAudioThread();
    void runTask();
    void playYTUrl(std::string);
};

#endif