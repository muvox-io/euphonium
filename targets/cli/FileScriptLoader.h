#ifndef FILE_SCRIPT_LOADER_H
#define FILE_SCRIPT_LOADER_H
#include <ScriptLoader.h>

class FileScriptLoader : public ScriptLoader
{
public:
    FileScriptLoader();
    ~FileScriptLoader() {}
    void loadScript(std::string scriptName, std::shared_ptr<sol::state> luaState);

private:
};

#endif