#include <FileScriptLoader.h>
#include <sys/stat.h>

FileScriptLoader::FileScriptLoader() {
    struct stat st;
    if (stat("../../../euphonium/scripts/configuration", &st) != 0 || !S_ISDIR(st.st_mode))
    {
        mkdir("../../../euphonium/scripts/configuration", 0755);
    }
}

void FileScriptLoader::loadScript(std::string scriptName, std::shared_ptr<berry::VmState> berry) {
    BELL_LOG(info, "fileloader", "Loading script: %s ", scriptName.c_str());
    auto content = loadFile("../../../euphonium/scripts/" + scriptName);
    if (!berry->execute_string(content)) {
        EUPH_LOG(error, "script_loader", "Failed to load script %s", scriptName.c_str());
    }
}

std::string FileScriptLoader::loadFile(std::string fileName) {
    std::ifstream indexFile(fileName);
    std::string indexContent((std::istreambuf_iterator<char>(indexFile)),
            std::istreambuf_iterator<char>());

    return indexContent;
}

void FileScriptLoader::saveFile(const std::string& fileName, const std::string& content) {
    // Save the file
    std::ofstream indexFile("../../../euphonium/scripts/" + fileName);
    indexFile << content;
    indexFile.close();
}
