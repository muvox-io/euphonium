#include <iostream>
#include <optional>
#include "Core.h"
#include "FileScriptLoader.h"

int main(int argc, char *argv[])
{
    auto core = std::make_shared<Core>();
    auto loader = std::make_shared<FileScriptLoader>();
    core->loadPlugins(loader);
    return 0;
}