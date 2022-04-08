#ifndef SPIFFS_SCRIPT_LOADER_H
#define SPIFFS_SCRIPT_LOADER_H
#include "Core.h"
#include <ScriptLoader.h>
#include <fstream>
#include <iostream>
#include <memory>

class SPIFFSScriptLoader : public ScriptLoader {
  public:
    SPIFFSScriptLoader(std::weak_ptr<Core> core);
    ~SPIFFSScriptLoader() {}
    void loadScript(std::string scriptName,
                    std::shared_ptr<berry::VmState> berry);
    std::string loadFile(std::string fileName);
    void saveFile(const std::string &fileName, const std::string &content);

  private:
    std::weak_ptr<Core> core;
};

#endif
