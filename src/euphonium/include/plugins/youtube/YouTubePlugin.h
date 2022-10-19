#ifndef EUPHONIUM_YOUTUBE_PLUGIN_H
#define EUPHONIUM_YOUTUBE_PLUGIN_H

#include "ScriptLoader.h"
#include "EuphoniumLog.h"
#include "Logger.h"
#include "CoreEvents.h"
#include "Module.h"
#include <atomic>
#include <mutex>
#include "Queue.h"
#include "../http/HTTPModule.h"
#include "MpegDashDemuxer.h"
#include "DIALServer.h"
#include "SSDPListener.h"
#include "YouTubeLeanHandler.h"
#include "Task.h"
#include "HTTPStream.h"
#include "aacdec.h"
#include "DecoderGlobals.h"
#include "TLSSocket.h"

// Could've done it with a JSON parser, but wanted to save overhead
#define POST_BODY_PART1 "{\"context\":{\"client\":{\"hl\":\"en\",\"clientName\":\"Android\",\"clientVersion\":\"15.47.38\",\"deviceMake\":\"Google\",\"deviceModel\":\"Android SDK built for x86\",\"osType\":\"Android\",\"osVersion\":\"7.1.1\"}},\"videoId\":\""
#define POST_BODY_PART2 "\"}"

#define YT_PLAYER_KEY "AIzaSyAO_FJ2SlqU8Q4STEHLGCilw_Y9_11qcW8"

class YouTubePlugin : public bell::Task, public Module
{
private:
    std::atomic<bool> isRunning;
    std::mutex runningMutex;
    bell::Queue<std::string> ytUrlQueue;
    std::shared_ptr<DIALServer> dialServer;

    std::shared_ptr<SSDPListener> ssdpListener;
    std::shared_ptr<YouTubeLeanHandler> leanHandler;
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
