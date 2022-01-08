#include "Core.h"
#include "EventBus.h"
#include <EuphoniumLog.h>
#include <cassert>
#include <string.h>

std::shared_ptr<MainAudioBuffer> mainAudioBuffer;
std::shared_ptr<EventBus> mainEventBus;

Core::Core() : bell::Task("Core", 4 * 1024, 10, 0) {
    audioBuffer = std::make_shared<MainAudioBuffer>();
    luaEventBus = std::make_shared<EventBus>();
    mainPersistor = std::make_shared<ConfigPersistor>();
    mainAudioBuffer = audioBuffer;
    mainEventBus = luaEventBus;

    audioProcessor = std::make_shared<AudioProcessors>();

    // Add preincluded audio processors
    audioProcessor->addProcessor(std::make_unique<SoftwareVolumeProcessor>());
    audioProcessor->addProcessor(std::make_unique<EqualizerProcessor>());

    // include berry
    berry = std::make_shared<berry::VmState>();

    // Import necessary berry modules
    berry->execute_string("import json");
    berry->execute_string("import global");

    // Prepare lua event thread
    auto subscriber = dynamic_cast<EventSubscriber *>(this);
    luaEventBus->addListener(EventType::LUA_MAIN_EVENT, *subscriber);

    registeredPlugins = {
        std::make_shared<CSpotPlugin>(), std::make_shared<WebRadioPlugin>(),
        // std::make_shared<YouTubePlugin>()
    };
    requiredModules = {std::make_shared<HTTPModule>(), mainPersistor};

    audioBuffer->shutdownListener = [this](std::string exceptPlugin) {
        EUPH_LOG(info, "core", "Shutting down except %s", exceptPlugin.c_str());

        // Notify scripting part about audio source change
        auto event = std::make_unique<AudioTakeoverEvent>(exceptPlugin);
        this->luaEventBus->postEvent(std::move(event));

        for (auto &plugin : registeredPlugins) {
            if (plugin->name != exceptPlugin &&
                plugin->status == ModuleStatus::RUNNING) {
                EUPH_LOG(info, "core", "Shutting down %s",
                         plugin->name.c_str());
                plugin->shutdown();
            }
        }
    };
}

void Core::loadPlugins(std::shared_ptr<ScriptLoader> loader) {
    std::vector<std::string> berryModules({"app"});

    audioProcessor->setBindings(berry);

    for (auto const &module : this->requiredModules) {
        module->luaEventBus = this->luaEventBus;
        module->berry = berry;
        module->setupBindings();
        module->loadScript(loader);
    }

    for (auto const &value : berryModules) {
        loader->loadScript(value, berry);
    }

    currentOutput->setupBindings(berry);

    for (auto const &plugin : this->registeredPlugins) {
        EUPH_LOG(info, plugin->name, "Initializing");
        plugin->berry = this->berry;
        plugin->luaEventBus = this->luaEventBus;
        plugin->setupBindings();
        plugin->loadScript(loader);
    }

    berry->execute_string("loadPlugins()");

}

void Core::handleScriptingThread() {
    startTask();

    EUPH_LOG(info, "core", "Scripting thread listening");
    while (true) {
        if (!luaEventBus->update()) {
            BELL_SLEEP_MS(30);
        };
    }
}

void Core::selectAudioOutput(std::shared_ptr<AudioOutput> output) {
    mainAudioBuffer->audioOutput = output;
    currentOutput = output;
    this->outputConnected = true;
}

void Core::emptyBuffers() { audioBuffer->audioBuffer->emptyBuffer(); }

void Core::handleEvent(std::unique_ptr<Event> event) {
    EUPH_LOG(debug, "core", "Got event");
    // Load function
    berry->get_global("handleEvent");

    // Arg 1
    berry->string(event->subType);

    // Arg 2
    berry->map(event->toBerry());

    berry->pcall(2);

    if (be_top(berry->raw_ptr()) > 0) {
        BELL_LOG(error, "core",
                 "Berry stack invalid, possible memory leak (%d > 0 !)",
                 be_top(berry->raw_ptr()));
    }
}

void Core::startAudioThreadForPlugin(std::string pluginName,
                                     berry::map config) {
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

void sleepMS(int ms) { BELL_SLEEP_MS(ms); }

std::string Core::getPlatform() {
#ifdef ESP_PLATFORM
    return "esp32";
#else
    return "desktop";
#endif
}

std::string Core::getVersion() {
#ifdef EUPH_VERSION
    return std::string(EUPH_VERSION);
#else
    return "n/a";
#endif
}

void Core::setupBindings() {
    berry->export_this("start_plugin_thread", this,
                       &Core::startAudioThreadForPlugin);
    berry->export_function("sleep_ms", &sleepMS);
    berry->export_this("core_empty_buffers", this, &Core::emptyBuffers);
    berry->export_this("get_version", this, &Core::getVersion);
    berry->export_this("get_platform", this, &Core::getPlatform);
}

void Core::runTask() {
    EUPH_LOG(info, "core", "Audio output started");
    std::vector<uint8_t> pcmBuf(PCMBUF_SIZE);

    while (true) {
        if (audioBuffer->audioBuffer->size() > 0 && outputConnected) {
            auto readNumber =
                audioBuffer->audioBuffer->read(pcmBuf.data(), PCMBUF_SIZE);
            audioProcessor->process(pcmBuf.data(), readNumber);
            currentOutput->feedPCMFrames(pcmBuf.data(), readNumber);
        } else {
            // EUPH_LOG(info, "core", "No data");
            BELL_SLEEP_MS(1000);
        }
    }
}
