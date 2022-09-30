#include "HTTPDevTools.h"

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

void HTTPDevTools::registerHandlers(std::shared_ptr<bell::HTTPServer> server) {


    auto directoriesHandler = [](bell::HTTPRequest &request) {
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
    auto uploadFileHandler = [](bell::HTTPRequest &request) {
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

    // mainServer->registerHandler(bell::RequestType::GET, "/devtools/file",
    //                             directoriesHandler);
    // mainServer->registerHandler(bell::RequestType::GET, "/logs",
    //                             requestLogsHandler);
    // mainServer->registerHandler(bell::RequestType::POST, "/file/*)",
    //                             uploadFileHandler);

    mainServer->registerHandler(bell::RequestType::POST, "/packages/upload)",
                                uploadFileHandler);
}