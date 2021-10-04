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
    luaEventBus->addListener(EventType::EVENT_FETCH_SERVICES, *subscriber);

    std::unique_ptr<Event> eventPtr(new Event);
    eventPtr->eventType = EventType::EVENT_FETCH_SERVICES;

    luaEventBus->postEvent(std::move(eventPtr));
    luaEventBus->update();
    luaState = std::make_shared<sol::state>();
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
    luaState->open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table);
    std::vector<std::string> luaModules({"json", "app"});

    for (auto const &module : this->requiredModules)
    {
        std::cout << "[" << module->name << "]: Initializing" << std::endl;
        module->setupLuaBindings(this->luaState);
        module->loadScript(loader, this->luaState);
    }

    for (auto const &value : luaModules)
    {
        loader->loadScript(value, luaState);
    }

    for (auto const &plugin : this->registeredPlugins)
    {
        std::cout << "[" << plugin->name << "]: Initializing" << std::endl;
        plugin->setupLuaBindings(this->luaState);
        plugin->loadScript(loader, this->luaState);
    }

    checkResult(luaState->script("app:printRegisteredPlugins()"));
}

void Core::selectAudioOutput(std::shared_ptr<AudioOutput> output)
{
    currentOutput = output;
}

void Core::handleEvent(std::unique_ptr<Event> event) {
    std::cout << "Got event!" << std::endl;
}

void Core::startAudioThreadForPlugin(std::string pluginName) {
    for (auto const &plugin : this->registeredPlugins) {
        if (plugin->name == pluginName) {
            std::cout << "[" << plugin->name << "]: Starting audio thread" << std::endl;
            plugin->startAudioThread();
            return;
        }
    }
}

void Core::setupBindings() {
    luaState->set_function("startAudioThreadForPlugin", &Core::startAudioThreadForPlugin, this);
}

void Core::handleAudioOutputThread() {
    while (true) {
        if (audioBuffer->size() > 0 && outputConnected) {
            this->currentOutput->update(audioBuffer);
        }
    }
}