#ifndef EUPHONIUM_SCRIPT_LOADER_H
#define EUPHONIUM_SCRIPT_LOADER_H

#include <string>
#include <sol.hpp>

class ScriptLoader {
public:
    ScriptLoader() {};
    virtual void loadScript(std::string scriptName, lua_State* luaState) = 0;
    virtual std::string loadFile(std::string fileName) = 0;
};

#endif