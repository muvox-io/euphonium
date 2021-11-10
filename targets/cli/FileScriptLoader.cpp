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

std::string FileScriptLoader::loadFile(std::string fileName) {
    std::ifstream indexFile(fileName);
    std::string indexContent((std::istreambuf_iterator<char>(indexFile)),
            std::istreambuf_iterator<char>());

    return indexContent;
}