#ifndef SPIFFS_SCRIPT_LOADER_H
#define SPIFFS_SCRIPT_LOADER_H
#include <ScriptLoader.h>
#include <iostream>
#include <fstream>

class SPIFFSScriptLoader : public ScriptLoader
{
public:
    SPIFFSScriptLoader();
    ~SPIFFSScriptLoader() {}
    void loadScript(std::string scriptName, std::shared_ptr<Berry> berry);
    std::string loadFile(std::string fileName);

private:
};

#endif
