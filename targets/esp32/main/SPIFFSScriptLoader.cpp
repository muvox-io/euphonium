#include <SPIFFSScriptLoader.h>

SPIFFSScriptLoader::SPIFFSScriptLoader()
{
}

void SPIFFSScriptLoader::loadScript(std::string scriptName, lua_State *luaState)
{
    sol::state_view lua(luaState);
    std::ifstream indexFile("/spiffs/" + scriptName + ".lua");
    std::string scriptContent((std::istreambuf_iterator<char>(indexFile)),
                             std::istreambuf_iterator<char>());
    auto result = lua.script(scriptContent);
    if (!result.valid())
    {
        std::cout << ((sol::error)result).what() << std::endl;
    }
}

std::string SPIFFSScriptLoader::loadFile(std::string fileName)
{
    std::ifstream indexFile(fileName);
    std::string indexContent((std::istreambuf_iterator<char>(indexFile)),
                             std::istreambuf_iterator<char>());

    return indexContent;
}