#include "CSpotPlugin.h"

#include <thread>

ESP32Platform::ESP32Platform()
{

}

void ESP32Platform::loadScript(std::shared_ptr<ScriptLoader> scriptLoader)
{
    // scriptLoader->loadScript("cspot_plugin", luaState);
}

void ESP32Platform::setupLuaBindings()
{
    sol::state_view lua(luaState);
    lua.set_function("cspotConfigUpdated", &CSpotPlugin::configurationUpdated, this);
}

void ESP32Platform::configurationUpdated()
{

    //startAudioThread();
}


void ESP32Platform::startAudioThread()
{
}