#ifndef EUPHONIUM_CORE_H
#define EUPHONIUM_CORE_H

#include <vector>
#include <memory>
#include <iostream>
#include "AudioOutput.h"
#include "plugins/http/HTTPModule.h"
#include "plugins/cspot/CSpotPlugin.h"
#include "plugins/webradio/WebRadioPlugin.h"
#include "MainAudioBuffer.h"
#include "Module.h"
#include "ScriptLoader.h"
#include "EventBus.h"
#include <thread>
class Core: public bell::Task, public EventSubscriber {
private:
    std::shared_ptr<AudioOutput> currentOutput;
    sol::state luaState;
    std::vector<std::shared_ptr<Module>> requiredModules;
    std::vector<std::shared_ptr<Module>> registeredPlugins;
    bool outputConnected = false;

public:
    Core();
    ~Core() {};
    void selectAudioOutput(std::shared_ptr<AudioOutput> output);
    void loadPlugins(std::shared_ptr<ScriptLoader> loader);
    void handleLuaThread();
    void handleServerThread();
    void runTask();
    void handleEvent(std::unique_ptr<Event> event);
    void setupBindings();
    void startAudioThreadForPlugin(std::string pluginName, sol::table config);

    std::shared_ptr<EventBus> luaEventBus;
    std::shared_ptr<MainAudioBuffer> audioBuffer;
};

#endif