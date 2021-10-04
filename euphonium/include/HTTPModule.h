#ifndef EUPHONIUM_HTTP_MODULE_H
#define EUPHONIUM_HTTP_MODULE_H

#include "ScriptLoader.h"
#include <sol.hpp>

#include "HTTPServer.h"
#include "Module.h"

class HTTPModule: public Module {
    public:
    HTTPModule();
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader, std::shared_ptr<sol::state> luaState);
    void setupLuaBindings(std::shared_ptr<sol::state> luaState);
    void startAudioThread();
};

#endif