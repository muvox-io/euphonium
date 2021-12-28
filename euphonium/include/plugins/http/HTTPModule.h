#ifndef EUPHONIUM_HTTP_MODULE_H
#define EUPHONIUM_HTTP_MODULE_H

#include "ScriptLoader.h"
#include <iostream>
#include <fstream>
#include <thread>
#include "Module.h"
#include "Task.h"
#include "EuphoniumLog.h"
#include "dirent.h"
#include "plugins/persistor/ConfigPersistor.h"
#include "HTTPServer.h"

extern std::shared_ptr<bell::HTTPServer> mainServer;
class HTTPModule : public bell::Task, public Module
{
private:
    bool taskRunning = false;
public:
    HTTPModule();
    std::shared_ptr<ScriptLoader> scriptLoader;
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader);
    void setupBindings();
    void registerHandler(std::string routeUrl, const std::string reqType, int handlerId);
    void respond(int connectionFd, int status, std::string body, std::string contentType);
    void runTask();
    void publishEvent(std::string eventName, std::string eventData);
    void startAudioThread();
    void listen();
    void shutdown(){};
};

#endif
