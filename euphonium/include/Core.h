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
#include "UserDSPProcessor.h"
#include <thread>
#include "BerryBind.h"

// Plugins 
#include "plugins/http/HTTPModule.h"
#include "plugins/cspot/CSpotPlugin.h"
#include "plugins/webradio/WebRadioPlugin.h"
#include "plugins/youtube/YouTubePlugin.h"
#include "plugins/persistor/ConfigPersistor.h"
#include "plugins/mqtt/MQTTPlugin.h"

#define PCMBUF_SIZE (1024*4)

extern std::shared_ptr<MainAudioBuffer> mainAudioBuffer;
extern std::shared_ptr<EventBus> mainEventBus;

class Core: public bell::Task, public EventSubscriber {
private:
    std::shared_ptr<AudioOutput> currentOutput;
    std::shared_ptr<AudioProcessors> audioProcessor;
    bool outputConnected = false;
    std::shared_ptr<berry::VmState> berry;
    std::shared_ptr<ScriptLoader> loader;

public:
    Core();
    ~Core() {};
    std::vector<std::shared_ptr<Module>> requiredModules;
    std::vector<std::shared_ptr<Module>> registeredPlugins;

    void selectAudioOutput(std::shared_ptr<AudioOutput> output);
    void loadPlugins(std::shared_ptr<ScriptLoader> loader);
    void handleScriptingThread();
    void runTask();
    void emptyBuffers();
    void loadScript(std::string file);
    void handleEvent(std::unique_ptr<Event> &event);
    void setupBindings();
    std::string getPlatform();
    std::string getVersion();
    void startAudioThreadForPlugin(std::string pluginName, berry::map config);

    std::shared_ptr<EventBus> luaEventBus;
    std::shared_ptr<MainAudioBuffer> audioBuffer;
};

#endif
