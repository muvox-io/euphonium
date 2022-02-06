#include "HTTPModule.h"
#include "BellLogger.h"
#include "EuphoniumLog.h"
#include "HTTPEvents.h"
#include "cJSON.h"

void listFiles(const std::string &path,
               std::function<void(const std::string &)> cb) {
    if (auto dir = opendir(path.c_str())) {
        while (auto f = readdir(dir)) {
            if (!f->d_name || f->d_name[0] == '.')
                continue;
            if (f->d_type == DT_DIR)
                listFiles(path + f->d_name + "/", cb);

            if (f->d_type == DT_REG)
                cb(path + f->d_name);
        }
        closedir(dir);
    }
}

HTTPModule::HTTPModule() : bell::Task("http", 1024 * 6, 0, 0, false) {
    name = "http";
    mainServer = std::make_shared<bell::HTTPServer>(80);
}

void HTTPModule::loadScript(std::shared_ptr<ScriptLoader> loader) {
    loader->loadScript("http", berry);
    this->scriptLoader = loader;
}

void HTTPModule::setupBindings() {
    BELL_LOG(info, "http", "Registering handlers");
    berry->export_this("_http_register_handler", this,
                       &HTTPModule::registerHandler);
    berry->export_this("_http_respond", this, &HTTPModule::respond);
    berry->export_this("_http_publish_event", this, &HTTPModule::publishEvent);
}

void HTTPModule::publishEvent(std::string event, std::string data) {
    mainServer->publishEvent(event, data);
}

void HTTPModule::respond(int connectionFd, int status, std::string body,
                         std::string contentType) {
    BELL_LOG(info, "http", "Responding to connection");
    BELL_LOG(info, "http", "Status: %d", status);
    BELL_LOG(info, "http", "Body: %s", body.c_str());
    bell::HTTPResponse response = {.connectionFd = connectionFd,
                                   .status = status,
                                   .body = body,
                                   .contentType = contentType};

    mainServer->respond(response);
}

void HTTPModule::registerHandler(std::string routeUrl, std::string requestType,
                                 int handlerId) {
    auto handler = [handlerId, this](bell::HTTPRequest &request) {
        request.handlerId = handlerId;
        auto event = std::make_unique<HandleRouteEvent>(request);
        EUPH_LOG(info, "http", "Posting the event");
        this->luaEventBus->postEvent(std::move(event));
    };

    EUPH_LOG(debug, "http", "Registering handler for %s", routeUrl.c_str());
    auto reqType = bell::RequestType::GET;
    if (requestType == "POST") {
        reqType = bell::RequestType::POST;
    }
    mainServer->registerHandler(reqType, routeUrl, handler);
}

void HTTPModule::runTask() {
    if (taskRunning)
        return;
    taskRunning = true;
    EuphoniumLogger *logger =
        static_cast<EuphoniumLogger *>(bell::bellGlobalLogger.get());

    auto assetHandler = [this](bell::HTTPRequest &request) {
        auto fileName = request.urlParams.at("asset");
        std::string fullFilePath = "assets/" + fileName;
        mainPersistor->serveFile(request.connection, fullFilePath);
    };
    auto directoriesHandler = [this](bell::HTTPRequest &request) {
        std::string result = "[";
        std::string prefix = "../../../euphonium/scripts/";
#ifdef ESP_PLATFORM
        prefix = "/spiffs/";
#endif
        listFiles(prefix, [&result, prefix](const std::string &file) {
            BELL_LOG(info, "http", "%s", file.c_str());
            std::string path = file;
            if (path.find(prefix) != std::string::npos) {
                path.erase(0, prefix.size() - 1);
            }

            result += "\"" + path + "\",";
        });

        result.pop_back();
        result += "]";
        bell::HTTPResponse response = {
            .connectionFd = request.connection,
            .status = 200,
            .body = result,
            .contentType = "application/json",
        };
        mainServer->respond(response);
    };
    auto uploadFileHandler = [this](bell::HTTPRequest &request) {
        BELL_LOG(info, "http", "Received file update of len %d",
                 request.body.size());
        BELL_LOG(info, "http", "File name: %s",
                 request.url.substr(15, request.url.size()).c_str());
        mainPersistor->persist(request.url.substr(15, request.url.size()),
                               request.body);
        bell::HTTPResponse response = {
            .connectionFd = request.connection,
            .status = 200,
            .body = "{ \"status\": \"ok\"}",
            .contentType = "application/json",
        };
        mainServer->respond(response);
    };
    auto requestLogsHandler = [&logger](bell::HTTPRequest &request) {
        std::string res;
        for (auto &log : logger->logCache) {
            res += log;
        }
        bell::HTTPResponse response = {
            .connectionFd = request.connection,
            .status = 200,
            .body = res,
            .contentType = "application/json",
        };
        mainServer->respond(response);
    };

    auto indexHandler = [this](bell::HTTPRequest &request) {
        std::string fileName = "index.html";
        if (request.url.find("/devtools/file") != std::string::npos) {
            fileName = request.url.substr(request.url.find("/devtools") + 15,
                                          request.url.size());
        }
        mainPersistor->serveFile(request.connection, fileName);
    };

    auto restartHandler = [this](bell::HTTPRequest &request) {
        bell::HTTPResponse response = {
            .connectionFd = request.connection,
            .status = 200,
            .body = "{ \"status\": \"ok\"}",
            .contentType = "application/json",
        };
        mainServer->respond(response);
#ifdef ESP_PLATFORM
        esp_restart();
#endif
    };

    auto renameFileHandler = [this](bell::HTTPRequest &request) {
        auto body = request.body;
        // parse body with cjson
        cJSON *root = cJSON_Parse(body.c_str());
        if (!root) {
            BELL_LOG(error, "http", "Error parsing JSON");
        }

        std::string oldName =
            cJSON_GetObjectItem(root, "currentName")->valuestring;
        std::string newName = cJSON_GetObjectItem(root, "newName")->valuestring;

        cJSON_Delete(root);
        std::string prefix = "../../../euphonium/scripts/";
#ifdef ESP_PLATFORM
        prefix = "/spiffs/";
#endif
        std::rename(std::string(prefix + oldName).c_str(),
                    std::string(prefix + newName).c_str());

        bell::HTTPResponse response = {
            .connectionFd = request.connection,
            .status = 200,
            .body = "{ \"status\": \"ok\"}",
            .contentType = "application/json",
        };
        mainServer->respond(response);
    };

    // redirect all requests to /web
    auto rootHandler = [this](bell::HTTPRequest &request) {
        mainServer->redirectTo("/web", request.connection);
    };

    // mainServer->registerHandler(bell::RequestType::GET, "/assets/:asset",
    // assetHandler);
    mainServer->registerHandler(bell::RequestType::GET, "/web/*", indexHandler);
    mainServer->registerHandler(bell::RequestType::GET, "/assets/:asset",
                                assetHandler);
    mainServer->registerHandler(bell::RequestType::GET, "/devtools/file/*",
                                indexHandler);
    mainServer->registerHandler(bell::RequestType::GET, "/devtools/file",
                                directoriesHandler);
    mainServer->registerHandler(bell::RequestType::GET, "/devtools/logs",
                                requestLogsHandler);
    mainServer->registerHandler(bell::RequestType::GET, "/", rootHandler);
    mainServer->registerHandler(bell::RequestType::POST, "/devtools/file/*)",
                                uploadFileHandler);
    mainServer->registerHandler(bell::RequestType::POST,
                                "/devtools/rename-file", renameFileHandler);
    mainServer->registerHandler(bell::RequestType::POST,
                                "/system/restart", restartHandler);
    mainServer->listen();
}

void HTTPModule::startAudioThread() {
    if (taskRunning)
        return;
    startTask();
}
