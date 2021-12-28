#include "HTTPModule.h"
#include "BellLogger.h"
#include "EuphoniumLog.h"
#include "HTTPEvents.h"

std::shared_ptr<bell::HTTPServer> mainServer;

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

HTTPModule::HTTPModule() : bell::Task("http", 1024 * 4, 0, 0, false) {
    name = "http";
    mainServer = std::make_shared<bell::HTTPServer>(80);
}

void HTTPModule::loadScript(std::shared_ptr<ScriptLoader> loader) {
    loader->loadScript("http", berry);
    this->scriptLoader = loader;
}

void HTTPModule::setupBindings() {
    BELL_LOG(info, "http", "Registering handlers");
    berry->export_this("httpRegisterHandler", this,
                       &HTTPModule::registerHandler);
    berry->export_this("httpRespond", this, &HTTPModule::respond);
    berry->export_this("httpPublishEvent", this, &HTTPModule::publishEvent);
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
    EuphoniumLogger* logger = static_cast<EuphoniumLogger *>(bell::bellGlobalLogger.get());

    auto assetHandler = [this](bell::HTTPRequest &request) {
        auto fileName = request.urlParams.at("asset");
        auto extension = fileName.substr(fileName.size() - 3, fileName.size());
        auto useGzip = false;
        auto contentType = "text/css";
        if (extension == ".js") {
            contentType = "application/javascript";
        }

        std::string prefix = "../../../web/dist/";
        std::string checkPrefix = prefix;
#ifdef ESP_PLATFORM
        prefix = "/spiffs/";
        checkPrefix = "/spiffs/";
#endif

        std::ifstream checkFile(checkPrefix + "assets/" + fileName + ".gz");
        if (checkFile.good()) {
            fileName += ".gz";
            useGzip = true;
        }

        bell::HTTPResponse response = {
            .connectionFd = request.connection,
            .status = 200,
            .useGzip = useGzip,
            .contentType = contentType,
        };
        response.responseReader = std::make_unique<bell::FileResponseReader>(
            prefix + "assets/" + fileName);
        mainServer->respond(response);
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
        BELL_LOG(info, "http", "File name: %s", request.url.c_str());
        mainPersistor->persist(request.url.substr(6, request.url.size()),
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

    auto rebootHandler = [&logger](bell::HTTPRequest &request) {
        bell::HTTPResponse response = {
            .connectionFd = request.connection,
            .status = 200,
            .body = "{\"status\": \"ok\"}",
            .contentType = "application/json",
        };
        mainServer->respond(response);

#ifdef ESP_PLATFORM
        esp_restart();
#endif
    };

    auto indexHandler = [this](bell::HTTPRequest &request) {
        std::string prefix = "../../../web/dist/";
        std::string fileName = "index.html";
        std::string contentType = "text/html";
        if (request.url.find("/file/") != std::string::npos) {
            prefix = "../../../euphonium/scripts/";
            fileName = request.url.substr(request.url.find("/file/") + 6,
                                          request.url.size());
            contentType = "text/plain";
        }

#ifdef ESP_PLATFORM
        prefix = "/spiffs/";
#endif

        BELL_LOG(info, "http", "Got url %s", request.url.c_str());

        BELL_LOG(info, "http", "Loading file %s", fileName.c_str());
        std::ifstream checkFile(prefix + fileName);
        if (checkFile.good()) {
            bell::HTTPResponse response = {
                .connectionFd = request.connection,
                .status = 200,
                .contentType = contentType,
            };
            response.responseReader =
                std::make_unique<bell::FileResponseReader>(prefix + fileName);
            mainServer->respond(response);
        } else {
            bell::HTTPResponse response = {
                .connectionFd = request.connection,
                .status = 404,
                .body = "File not found",
                .contentType = "text/html",
            };
            mainServer->respond(response);
        }
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
    mainServer->registerHandler(bell::RequestType::GET, "/file/*",
                                indexHandler);
    mainServer->registerHandler(bell::RequestType::GET, "/directories",
                                directoriesHandler);
    mainServer->registerHandler(bell::RequestType::GET, "/request_logs",
                                requestLogsHandler);
    mainServer->registerHandler(bell::RequestType::GET, "/reboot",
                                rebootHandler);
    mainServer->registerHandler(bell::RequestType::GET, "/", rootHandler);
    mainServer->registerHandler(bell::RequestType::POST, "/file/*)",
                                uploadFileHandler);
    mainServer->listen();
}

void HTTPModule::startAudioThread() {
    if (taskRunning)
        return;
    startTask();
}
