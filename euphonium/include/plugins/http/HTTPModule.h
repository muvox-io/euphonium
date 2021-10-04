#ifndef EUPHONIUM_HTTP_MODULE_H
#define EUPHONIUM_HTTP_MODULE_H

#include "ScriptLoader.h"
#include <sol.hpp>
#include <thread>
#include "Module.h"
#include "plugins/http/HTTPServer.h"

class HTTPModule: public Module {
    public:
    HTTPModule();
    std::shared_ptr<HTTPServer> server;
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader);
    void setupLuaBindings();
    void registerHandler(const std::string &routeUrl, RequestType reqType, int handlerId);
    void startAudioThread();
    void listen();
};

#endif