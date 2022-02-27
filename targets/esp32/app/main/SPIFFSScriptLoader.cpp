#include <SPIFFSScriptLoader.h>
#include <sys/stat.h>

SPIFFSScriptLoader::SPIFFSScriptLoader()
{
    struct stat st;
    if (stat("/spiffs/configuration", &st) != 0 || !S_ISDIR(st.st_mode))
    {
        mkdir("/spiffs/configuration", 0755);
    }
}

void SPIFFSScriptLoader::loadScript(std::string scriptName, std::shared_ptr<berry::VmState> berry)
{
    BELL_LOG(info, "spiffs_loader", "Loading script: %s", scriptName.c_str());
    auto scriptContent = loadFile(scriptName);
    if (!berry->execute_string(scriptContent))
    {
        EUPH_LOG(error, "script_loader", "Failed to load script %s", scriptName.c_str());
    }
}

std::string SPIFFSScriptLoader::loadFile(std::string fileName)
{
    std::ifstream indexFile("/spiffs/" + fileName);
    std::string indexContent((std::istreambuf_iterator<char>(indexFile)),
                             std::istreambuf_iterator<char>());

    return indexContent;
}

void SPIFFSScriptLoader::saveFile(const std::string& fileName, const std::string& content)
{
    // Save the file
    std::ofstream indexFile("/spiffs/" + fileName);
    indexFile << content;
    indexFile.close();
}
