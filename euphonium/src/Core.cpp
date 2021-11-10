#include "Core.h"
#include <string.h>
#define SOL_ALL_SAFETIES_ON 1
#include <sol.hpp>
#include <cassert>
#include <EuphoniumLog.h>

Core::Core()
{
    audioBuffer = std::make_shared<CircularBuffer>(AUDIO_BUFFER_SIZE);
    luaEventBus = std::make_shared<EventBus>();

    auto subscriber = dynamic_cast<EventSubscriber*>(this);
    luaEventBus->addListener(EventType::LUA_MAIN_EVENT, *subscriber);
    // luaState = std::make_shared<sol::state>();
    this->setupBindings();
    registeredPlugins = {
        std::make_shared<CSpotPlugin>()};
    requiredModules = {
        std::make_shared<HTTPModule>()};
}

void checkResult(sol::protected_function_result result)
{
    if (!result.valid())
    {
        std::cout << ((sol::error)result).what() << std::endl;
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

    std::thread newThread(&Core::handleAudioOutputThread, this);
    newThread.detach();

    EUPH_LOG(info, "core", "Lua thread listening");
    while(true) {
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
}

void Core::handleAudioOutputThread() {
    EUPH_LOG(info, "core", "Audio output started");
    while (true) {
        if (audioBuffer->size() > 0 && outputConnected) {
            this->currentOutput->update(audioBuffer);
        }
    }
}