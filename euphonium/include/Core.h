#ifndef EUPHONIUM_CORE_H
#define EUPHONIUM_CORE_H

#include <vector>
#include <memory>
#include <iostream>
#include "AudioOutput.h"
#include "MainAudioBuffer.h"
#include "Module.h"
#include "ScriptLoader.h"
#include "SoftwareVolumeProcessor.h"
#include "EventBus.h"
#include "AudioProcessors.h"
#include "EqualizerProcessor.h"
#include <thread>
#include "BerryBind.h"

// Plugins 
#include "plugins/http/HTTPModule.h"
#include "plugins/cspot/CSpotPlugin.h"
#include "plugins/webradio/WebRadioPlugin.h"
#include "plugins/youtube/YouTubePlugin.h"


#define PCMBUF_SIZE (1024*4)

class Core: public bell::Task, public EventSubscriber {
private:
    std::shared_ptr<AudioOutput> currentOutput;
    sol::state luaState;
    std::vector<std::shared_ptr<Module>> requiredModules;
    std::vector<std::shared_ptr<Module>> registeredPlugins;
    std::shared_ptr<AudioProcessors> audioProcessor;
    bool outputConnected = false;
    std::shared_ptr<Berry> berry;

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
    void startAudioThreadForPlugin(std::string pluginName, berry_map config);

    std::shared_ptr<EventBus> luaEventBus;
    std::shared_ptr<MainAudioBuffer> audioBuffer;
};

#endif
