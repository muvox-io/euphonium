#include "Core.h"
#include "FileScriptLoader.h"
#include "PortAudioAudioOutput.h"
#include <EuphoniumLog.h>
#include <iostream>
#include <optional>

int main(int argc, char *argv[]) {
    initializeEuphoniumLogger();
    bell::createDecoders();
    auto core = std::make_shared<Core>();
    auto loader = std::make_shared<FileScriptLoader>();
    auto output = std::make_shared<PortAudioAudioOutput>();
    core->selectAudioOutput(output);
    core->setupBindings();
    core->loadPlugins(loader);
    return 0;
}
