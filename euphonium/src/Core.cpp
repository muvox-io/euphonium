#include "Core.h"
#include <string.h>
#include <cassert>
#include <EuphoniumLog.h>

Core::Core() : bell::Task("Core", 4 * 1024, 0) {
    audioBuffer = std::make_shared<MainAudioBuffer>();
    luaEventBus = std::make_shared<EventBus>();
    audioProcessor = std::make_shared<AudioProcessors>();
    audioProcessor->addProcessor(std::make_unique<SoftwareVolumeProcessor>());
    audioProcessor->addProcessor(std::make_unique<EqualizerProcessor>());
    berry = std::make_shared<Berry>();
    berry->execute_string("import json");
    berry->execute_string("import global");

    // Prepare lua event thread
    auto subscriber = dynamic_cast<EventSubscriber*>(this);
    luaEventBus->addListener(EventType::LUA_MAIN_EVENT, *subscriber);

    BELL_LOG(info, "core", "Core initialized");
    this->setupBindings();
    BELL_LOG(info, "core", "Core initialized 2");
    registeredPlugins = {
        std::make_shared<CSpotPlugin>(),
        std::make_shared<WebRadioPlugin>(),
        std::make_shared<YouTubePlugin>()
    };
    requiredModules = {
        std::make_shared<HTTPModule>()};
    BELL_LOG(info, "core", "Core initialized 3");

    audioBuffer->shutdownListener = [this](std::string exceptPlugin) {
        for (auto& plugin : registeredPlugins) {
            if (plugin->name != exceptPlugin && plugin->status == ModuleStatus::RUNNING) {
                EUPH_LOG(info, "core", "Shutting down %s", plugin->name.c_str());
                plugin->shutdown();
            }
        }
    };
}

void Core::loadPlugins(std::shared_ptr<ScriptLoader> loader)
{
    std::vector<std::string> berryModules({"app"});

    audioProcessor->setBindings(berry);

    for (auto const &module : this->requiredModules)
    {
        module->luaEventBus = this->luaEventBus;
        module->berry = berry;
        module->setupBindings();
        module->loadScript(loader);
    }

    for (auto const &value : berryModules)
    {
        loader->loadScript(value, berry);
    }

    for (auto const &plugin : this->registeredPlugins)
    {
        EUPH_LOG(info, plugin->name, "Initializing");
        plugin->berry = this->berry;
        plugin->luaEventBus = this->luaEventBus;
        plugin->setupBindings();
        plugin->loadScript(loader);
    }
    
    berry->execute_string("printRegisteredPlugins()");

    startTask();

    EUPH_LOG(info, "core", "Lua thread listening");
    while(true) {
        BELL_SLEEP_MS(100);
        luaEventBus->update();
    }
}

void Core::selectAudioOutput(std::shared_ptr<AudioOutput> output)
{
    currentOutput = output;
    this->outputConnected = true;
}

void Core::handleEvent(std::unique_ptr<Event> event) {
    EUPH_LOG(debug, "core", "Got event");
    BELL_LOG(debug, "core", "pre %d", be_top(berry->rawPtr()));

    // Load function
    berry->get_global("handleEvent");

    // Arg 1
    berry->string(event->subType);
    
    // Arg 2
    berry->map(event->toBerry());

    berry->pcall(2);
    BELL_LOG(debug, "core", "dd %d", be_top(berry->rawPtr()));
}

void Core::startAudioThreadForPlugin(std::string pluginName, berry_map config) {
    for (auto const &plugin : this->registeredPlugins) {
        if (plugin->name == pluginName) {
            plugin->config = config;
            EUPH_LOG(info, plugin->name, "Starting audio thread");
            plugin->audioBuffer = audioBuffer;
            plugin->startAudioThread();
            return;
        }
    }

    for (auto const &module : this->requiredModules) {
        if (module->name == pluginName) {
            EUPH_LOG(info, module->name, "Starting audio thread");
            module->config = config;
            module->audioBuffer = audioBuffer;
            module->startAudioThread();
            return;
        }
    }
}

void Core::setupBindings() {

    berry->export_this("startAudioThreadForPlugin", this, &Core::startAudioThreadForPlugin);
}

void Core::runTask() {
    EUPH_LOG(info, "core", "Audio output started");
    std::vector<uint8_t> pcmBuf(PCMBUF_SIZE);

    while (true) {
        if (audioBuffer->audioBuffer->size() > 0 && outputConnected) {
            auto readNumber = audioBuffer->audioBuffer->read(pcmBuf.data(), PCMBUF_SIZE);
            audioProcessor->process(pcmBuf.data(), readNumber);
            currentOutput->feedPCMFrames(pcmBuf.data(), readNumber);
        } else {
            EUPH_LOG(info, "core", "No data");
            BELL_SLEEP_MS(1000);
        }
    }
}
