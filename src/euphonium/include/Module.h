#ifndef EUPHONIUM_MODULE_H
#define EUPHONIUM_MODULE_H

#include "ScriptLoader.h"
#include <map>
#include "EventBus.h"
#include "MainAudioBuffer.h"
#include "BerryBind.h"

enum class ModuleStatus {
    SHUTDOWN,
    RUNNING
};

class Module {
    public:
    Module() {}
    std::string name;
    ModuleStatus status = ModuleStatus::SHUTDOWN;
    std::shared_ptr<berry::VmState> berry;
    std::shared_ptr<EventBus> luaEventBus;
    std::shared_ptr<MainAudioBuffer> audioBuffer;

    berry::map config;
    virtual void loadScript(std::shared_ptr<ScriptLoader> scriptLoader) = 0;
    virtual void setupBindings() = 0;
    virtual void startAudioThread() = 0;
    virtual void shutdown() = 0;
};

#endif
