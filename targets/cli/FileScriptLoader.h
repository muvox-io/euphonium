#ifndef FILE_SCRIPT_LOADER_H
#define FILE_SCRIPT_LOADER_H
#include <ScriptLoader.h>
#include <iostream>
#include <fstream>

class FileScriptLoader : public ScriptLoader
{
public:
    FileScriptLoader();
    ~FileScriptLoader() {}
    void loadScript(std::string scriptName, std::shared_ptr<Berry> berry);
    std::string loadFile(std::string fileName);

private:
};

#endif