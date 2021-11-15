#ifndef EUPHONIUM_HTTP_MODULE_H
#define EUPHONIUM_HTTP_MODULE_H

#include "ScriptLoader.h"
#include <sol.hpp>
#include <iostream>
#include <fstream>
#include <thread>
#include "Module.h"
#include "EuphoniumLog.h"
#include "HTTPServer.h"

extern std::shared_ptr<bell::HTTPServer> mainServer;
class HTTPModule: public Module {
    public:
    HTTPModule();
    std::shared_ptr<ScriptLoader> scriptLoader;
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader);
    void setupLuaBindings();
    void registerHandler(const std::string &routeUrl, bell::RequestType reqType, int handlerId);
    void startAudioThread();
    void listen();
};

#endif