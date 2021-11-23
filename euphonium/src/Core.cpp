#include "Core.h"
#include <string.h>
#define SOL_ALL_SAFETIES_ON 1
#include <sol.hpp>
#include <cassert>
#include <EuphoniumLog.h>

Core::Core() : bell::Task("Core", 4 * 1024, 1)
{
    audioBuffer = std::make_shared<MainAudioBuffer>();
    luaEventBus = std::make_shared<EventBus>();

    // Prepare lua event thread
    auto subscriber = dynamic_cast<EventSubscriber*>(this);
    luaEventBus->addListener(EventType::LUA_MAIN_EVENT, *subscriber);

    this->setupBindings();
    registeredPlugins = {
        std::make_shared<CSpotPlugin>(),
        std::make_shared<WebRadioPlugin>()
    };
    requiredModules = {
        std::make_shared<HTTPModule>()};

    audioBuffer->shutdownListener = [this](std::string exceptPlugin) {
        for (auto& plugin : registeredPlugins) {
            if (plugin->name != exceptPlugin && plugin->status == ModuleStatus::RUNNING) {
                EUPH_LOG(info, "core", "Shutting down %s", plugin->name.c_str());
                plugin->shutdown();
            }
        }
    };
}

void checkResult(sol::protected_function_result result)
{
    if (!result.valid())
    {
        std::cout << ((sol::error)result).what() << std::endl;
    }
}

void Core::changeVolume(int volume) {
    if (outputConnected) {
        audioBuffer->setVolume(volume);
    }
}

void Core::loadPlugins(std::shared_ptr<ScriptLoader> loader)
{
    luaState.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::debug);
    std::vector<std::string> luaModules({"json", "app"});

    for (auto const &module : this->requiredModules)
    {
        EUPH_LOG(info, module->name, "Initializing");
        module->luaEventBus = this->luaEventBus;
        module->luaState = luaState;
        module->setupLuaBindings();
        module->loadScript(loader);
    }

    for (auto const &value : luaModules)
    {
        loader->loadScript(value, luaState);
    }

    for (auto const &plugin : this->registeredPlugins)
    {
        EUPH_LOG(info, plugin->name, "Initializing");
        plugin->luaState = this->luaState;
        plugin->luaEventBus = this->luaEventBus;
        plugin->setupLuaBindings();
        plugin->loadScript(loader);
    }
    
    checkResult(luaState.script("app:printRegisteredPlugins()"));

    startTask();

    EUPH_LOG(info, "core", "Lua thread listening");
    while(true) {
        usleep(10000);
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
    luaState["handleEvent"](event->luaEventType, event->toLua(luaState));
}

void Core::startAudioThreadForPlugin(std::string pluginName, sol::table config) {
    for (auto const &plugin : this->registeredPlugins) {
        if (plugin->name == pluginName) {
            EUPH_LOG(info, plugin->name, "Starting audio thread");
            plugin->config = config;
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
    luaState.set_function("startAudioThreadForPlugin", &Core::startAudioThreadForPlugin, this);
    luaState.set_function("luaLogError", luaLogError);
    luaState.set_function("luaLogDebug", luaLogDebug);
    luaState.set_function("luaLogInfo", luaLogInfo);
    luaState.set_function("luaLog", luaLog);
    luaState.set_function("setVolume", &Core::changeVolume, this);

    sol::usertype<PlaybackInfo> playbackType = luaState.new_usertype<PlaybackInfo>("PlaybackInfo", sol::constructors<PlaybackInfo()>());
    playbackType["albumName"] = &PlaybackInfo::albumName;
    playbackType["artistName"] = &PlaybackInfo::artistName;
    playbackType["icon"] = &PlaybackInfo::icon;
    playbackType["songName"] = &PlaybackInfo::songName;
    playbackType["sourceName"] = &PlaybackInfo::sourceName;
}

void Core::runTask() {
    EUPH_LOG(info, "core", "Audio output started");
    while (true) {
        if (audioBuffer->audioBuffer->size() > 0 && outputConnected) {
            this->currentOutput->update(audioBuffer->audioBuffer);
        } else {
            audioBuffer->audioBufferSemaphore->wait();
        }
    }
}