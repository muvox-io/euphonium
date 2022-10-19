#include "ConfigPersistor.h"

std::shared_ptr<ConfigPersistor> mainPersistor;

ConfigPersistor::ConfigPersistor()
    : bell::Task("persistor", 4 * 1024, 2, 0, false) {
    name = "persistor";
}

void ConfigPersistor::loadScript(std::shared_ptr<ScriptLoader> loader) {
    this->scriptLoader = loader;
}

void ConfigPersistor::persist(std::string key, std::string value) {
    StorageOperation request = {
        .type = StorageOperationType::PERSIST_CONF, .key = key, .value = value};

    requestQueue.push(request);
}

void ConfigPersistor::serveFile(int fd, std::string &fileName) {
    StorageOperation request = {.type = StorageOperationType::WRITE_HTTP,
                                .key = fileName,
                                .connFd = fd};

    requestQueue.push(request);
}

void ConfigPersistor::load(std::string key) {
    StorageOperation request = {
        .type = StorageOperationType::READ_CONF, .key = key, .value = ""};

    requestQueue.push(request);
}

void ConfigPersistor::setupBindings() {
    BELL_LOG(info, "persistor", "Registering handlers");
    berry->export_this("persist", this, &ConfigPersistor::persist, "persistor");
    berry->export_this("load", this, &ConfigPersistor::load, "persistor");
}

void ConfigPersistor::runTask() {
    StorageOperation request;
    while (true) {
        if (this->requestQueue.wpop(request)) {
            // save request.value to file named key
            if (request.type == StorageOperationType::PERSIST_CONF) {
                BELL_LOG(info, "persistor", "Saving key: %s",
                         request.key.c_str());
                scriptLoader->saveFile(request.key, request.value);
            } else if (request.type == StorageOperationType::WRITE_HTTP) {
                auto fileName = request.key;

                auto extension =
                    fileName.substr(fileName.size() - 3, fileName.size());
                auto useGzip = false;
                auto contentType = "text/plain";
                if (extension == ".js") {
                    contentType = "application/javascript";
                } else if (extension == "css") {
                    contentType = "text/css";
                } else if (extension == "tml") {
                    contentType = "text/html";
                }

                std::string prefix = "../../../web/dist/";
#ifdef ESP_PLATFORM
                prefix = "/spiffs/";
#endif
                BELL_LOG(info, "persistor", "Sending file: %s",
                         fileName.c_str());
                std::ifstream checkFile(prefix + fileName + ".gz");
                if (checkFile.good()) {
                    fileName += ".gz";
                    useGzip = true;
                    checkFile.close();
                } else {
                    checkFile.close();
                    checkFile.open(prefix + fileName);
                    if (!checkFile.good()) {
                        checkFile.close();
                        bell::HTTPResponse response = {
                            .connectionFd = request.connFd,
                            .status = 404,
                            .body = "404 Not Found",
                            .contentType = "text/plain",
                        };
                        mainServer->respond(response);
                        continue;
                    }
                }

                bell::HTTPResponse response = {
                    .connectionFd = request.connFd,
                    .status = 200,
                    .useGzip = useGzip,
                    .contentType = contentType,
                };
                response.responseReader =
                    std::make_unique<bell::FileResponseReader>(prefix +
                                                               fileName);
                mainServer->respond(response);
            } else {
                std::string prefix = "";
#ifndef ESP_PLATFORM
                prefix = "../../../euphonium/scripts/";
#endif
                std::string value =
                    scriptLoader->loadFile(prefix + request.key);
                BELL_LOG(info, "persistor", "Loaded key: %s",
                         request.key.c_str());
                auto event =
                    std::make_unique<ConfigLoadedEvent>(request.key, value);
                EUPH_LOG(info, "persistor", "Posting the event");
                this->luaEventBus->postEvent(std::move(event));
            }
        }
    }
}

void ConfigPersistor::startAudioThread() { startTask(); }
