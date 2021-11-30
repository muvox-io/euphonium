#ifndef EUPHONIUM_SCRIPT_LOADER_H
#define EUPHONIUM_SCRIPT_LOADER_H

#include <string>
#include <memory>
#include <BerryBind.h>

class ScriptLoader {
public:
    ScriptLoader() {};
    virtual void loadScript(std::string scriptName, std::shared_ptr<Berry> berry) = 0;
    virtual std::string loadFile(std::string fileName) = 0;
};

#endif
