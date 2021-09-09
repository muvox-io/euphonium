#ifndef EUPHONIUM_SCRIPT_LOADER_H
#define EUPHONIUM_SCRIPT_LOADER_H

#include <string>
#include <sol.hpp>

class ScriptLoader {
public:
    ScriptLoader() {};
    virtual void loadScript(std::string scriptName, std::shared_ptr<sol::state> luaState) = 0;
};

#endif