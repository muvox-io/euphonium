#ifndef EUPHONIUM_MODULE_H
#define EUPHONIUM_MODULE_H

#include "ScriptLoader.h"
#include <sol.hpp>

class Module {
    public:
    Module() {}
    std::string name;
    virtual void loadScript(std::shared_ptr<ScriptLoader> scriptLoader, std::shared_ptr<sol::state> luaState) = 0;
    virtual void setupLuaBindings(std::shared_ptr<sol::state> luaState) = 0;
    virtual void startAudioThread() = 0;
};

#endif