#include "CSpotPlugin.h"

CSpotPlugin::CSpotPlugin() {
    name = "cspot";
}

void CSpotPlugin::loadScript(std::shared_ptr<ScriptLoader> scriptLoader, std::shared_ptr<sol::state> luaState) {
    scriptLoader->loadScript("cspot_plugin", luaState);
}

void CSpotPlugin::setupLuaBindings(std::shared_ptr<sol::state> luaState) {
}