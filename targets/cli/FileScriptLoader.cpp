#include <FileScriptLoader.h>

FileScriptLoader::FileScriptLoader() {
}

void FileScriptLoader::loadScript(std::string scriptName, std::shared_ptr<berry::VmState> berry) {
    BELL_LOG(info, "fileloader", "Loading script: %s ", scriptName.c_str());
    auto content = loadFile("../../../euphonium/scripts/" + scriptName);
    auto scriptNameWithoutPath = scriptName.substr(scriptName.find_last_of("/") + 1);

    if (!berry->execute_string(content, scriptNameWithoutPath)) {
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
