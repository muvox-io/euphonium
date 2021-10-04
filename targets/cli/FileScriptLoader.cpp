#include <FileScriptLoader.h>

FileScriptLoader::FileScriptLoader() {
}

void FileScriptLoader::loadScript(std::string scriptName, lua_State* luaState) {
    sol::state_view lua(luaState);
    auto result = lua.script_file("../../../euphonium/lua/" + scriptName + ".lua");
    if (!result.valid())
    {
        std::cout << ((sol::error)result).what() << std::endl;
    }
}