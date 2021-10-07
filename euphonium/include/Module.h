#ifndef EUPHONIUM_MODULE_H
#define EUPHONIUM_MODULE_H

#include "ScriptLoader.h"
#include <sol.hpp>
#include <map>
#include "EventBus.h"
#include "CircularBuffer.h"

class Module {
    public:
    Module() {}
    std::string name;
    lua_State* luaState;
    std::shared_ptr<EventBus> luaEventBus;
    std::shared_ptr<CircularBuffer> audioBuffer;

    sol::table config;
    virtual void loadScript(std::shared_ptr<ScriptLoader> scriptLoader) = 0;
    virtual void setupLuaBindings() = 0;
    virtual void startAudioThread() = 0;
};

#endif