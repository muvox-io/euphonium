#include "Core.h"
#include <string.h>
#define SOL_ALL_SAFETIES_ON 1
#include <sol.hpp>
#include <cassert>

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
    luaState.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table);
    std::vector<std::string> luaModules({"json", "app"});

    for (auto const &module : this->requiredModules)
    {
        std::cout << "[" << module->name << "]: Initializing" << std::endl;
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
        std::cout << "[" << plugin->name << "]: Initializing" << std::endl;
        plugin->luaState = this->luaState;
        plugin->luaEventBus = this->luaEventBus;
        plugin->setupLuaBindings();
        plugin->loadScript(loader);
    }
    
    checkResult(luaState.script("app:printRegisteredPlugins()"));

    while(true) {
        luaEventBus->update();
    }
}

void Core::selectAudioOutput(std::shared_ptr<AudioOutput> output)
{
    currentOutput = output;
}

void Core::handleEvent(std::unique_ptr<Event> event) {
    std::cout << "Got event!" << std::endl;
    luaState["handleEvent"](event->luaEventType, event->toLua(luaState));
}

void Core::startAudioThreadForPlugin(std::string pluginName, sol::table config) {
    for (auto const &plugin : this->registeredPlugins) {
        if (plugin->name == pluginName) {
            std::cout << "[" << plugin->name << "]: Starting audio thread" << std::endl;
            plugin->config = config;
            plugin->startAudioThread();
            return;
        }
    }

    for (auto const &module : this->requiredModules) {
        if (module->name == pluginName) {
            std::cout << "[" << module->name << "]: Starting audio thread" << std::endl;
            module->config = config;
            module->startAudioThread();
            return;
        }
    }
}

void Core::setupBindings() {
    luaState.set_function("startAudioThreadForPlugin", &Core::startAudioThreadForPlugin, this);
}

void Core::handleAudioOutputThread() {
    while (true) {
        if (audioBuffer->size() > 0 && outputConnected) {
            this->currentOutput->update(audioBuffer);
        }
    }
}