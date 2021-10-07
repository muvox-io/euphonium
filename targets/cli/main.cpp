#include <iostream>
#include <optional>
#include "Core.h"
#include "FileScriptLoader.h"
#include "PortAudioAudioOutput.h"

int main(int argc, char *argv[])
{
    auto core = std::make_shared<Core>();
    auto loader = std::make_shared<FileScriptLoader>();
    auto output = std::make_shared<PortAudioAudioOutput>();
    core->selectAudioOutput(output);
    core->loadPlugins(loader);
    return 0;
}