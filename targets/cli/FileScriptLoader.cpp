#include <FileScriptLoader.h>

FileScriptLoader::FileScriptLoader() {
}

void FileScriptLoader::loadScript(std::string scriptName, std::shared_ptr<sol::state> luaState) {
    auto result = luaState->script_file("../../../euphonium/lua/" + scriptName + ".lua");
    if (!result.valid())
    {
        std::cout << ((sol::error)result).what() << std::endl;
    }
}