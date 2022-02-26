#ifndef EUPHONIUM_JELLYFIN_PLUGIN_H
#define EUPHONIUM_JELLYFIN_PLUGIN_H

#include "ScriptLoader.h"
#include "EuphoniumLog.h"
#include "Logger.h"
#include "CoreEvents.h"
#include "Module.h"
#include "Core.h"
#include <atomic>
#include <mutex>
#include <map>
#include "Queue.h"
#include "Task.h"
#include "JellyfinHTTPAudioStream.h"
#include <HTTPClient.h>

class JellyfinPlugin : public bell::Task, public Module
{
private:
    std::shared_ptr<JellyfinHTTPAudioStream> audioStream;
    std::atomic<bool> isRunning;
    std::atomic<bool> isPaused;
    std::mutex runningMutex;
    std::string token;
    std::string userid;
    std::string url;
    std::string username;
    std::string password;
    std::string deviceName;
    int login_status;
    std::shared_ptr<bell::HTTPClient> client;
    bell::Queue<std::pair<std::map<std::string, std::string>, std::string>> radioUrlQueue;

public:
    JellyfinPlugin();
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader);
    void setupBindings();
    void shutdown();
    void configurationUpdated();
    void setPaused(bool);
    void startAudioThread();
    void runTask();
    void login();
    void playSongByID(std::string trackid);
    void updateSongInfo(std::string trackid);
    std::string jellyfinURL(std::string path);
};

#endif
