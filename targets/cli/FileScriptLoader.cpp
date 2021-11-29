#include <FileScriptLoader.h>

FileScriptLoader::FileScriptLoader() {
}

void FileScriptLoader::loadScript(std::string scriptName, std::shared_ptr<Berry> berry) {
    BELL_LOG(info, "fileloader", "Loading script: %s ", scriptName.c_str());
    auto content = loadFile("../../../euphonium/scripts/" + scriptName + ".be");
    berry->execute_string(content);
}

std::string FileScriptLoader::loadFile(std::string fileName) {
    std::ifstream indexFile(fileName);
    std::string indexContent((std::istreambuf_iterator<char>(indexFile)),
            std::istreambuf_iterator<char>());

    return indexContent;
}