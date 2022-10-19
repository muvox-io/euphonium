#ifndef EUPHONIUM_SCRIPT_LOADER_H
#define EUPHONIUM_SCRIPT_LOADER_H

#include <string>
#include <memory>
#include <BerryBind.h>

class ScriptLoader {
public:
    ScriptLoader() {};
    virtual void loadScript(std::string scriptName, std::shared_ptr<berry::VmState> berry) = 0;
    virtual std::string loadFile(std::string fileName) = 0;
    virtual void saveFile(const std::string& fileName, const std::string& content) = 0;
};

#endif
