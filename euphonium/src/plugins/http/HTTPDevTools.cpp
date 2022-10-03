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

    //     auto directoriesHandler = [](bell::HTTPRequest &request) {
    //         std::string result = "[";
    //         std::string prefix = "../../../euphonium/scripts/";
    // #ifdef ESP_PLATFORM
    //         prefix = "/spiffs/";
    // #endif
    //         listFiles(prefix, [&result, prefix](const std::string &file) {
    //             BELL_LOG(info, "http", "%s", file.c_str());
    //             std::string path = file;
    //             if (path.find(prefix) != std::string::npos) {
    //                 path.erase(0, prefix.size() - 1);
    //             }

    //             result += "\"" + path + "\",";
    //         });

    //         result.pop_back();
    //         result += "]";
    //         bell::HTTPResponse response = {
    //             .connectionFd = request.connection,
    //             .status = 200,
    //             .body = result,
    //             .contentType = "application/json",
    //         };
    //         mainServer->respond(response);
    //     };
    auto uploadFileHandler = [](std::unique_ptr<bell::HTTPRequest> request) {
        // open file SCRIPTS_DIR/tmp/pkg.tar.gz
        // write request->body to file
        // close file
        // Save the file

        BELL_LOG(info, "http", "Received file update of len %d",
                 request->responseReader->getTotalSize());
        std::ofstream tmpPackage(SCRIPTS_PREFIX_PATH + "tmp/pkg.tar.gz",
                                std::ios::out | std::ios::binary);
        auto readBytes = 0;
        auto readBuffer = std::vector<uint8_t>(128);
        while (readBytes < request->responseReader->getTotalSize()) {
            auto read = request->responseReader->read((char*) readBuffer.data(), readBuffer.size());
            tmpPackage.write((char *)readBuffer.data(), read);
            readBytes += read;
        }
        tmpPackage.close();

        bell::HTTPResponse response = {
            .connectionFd = request->connection,
            .status = 200,
            .body = "{ \"status\": \"success\"}",
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

    mainServer->registerHandler(bell::RequestType::POST, "/packages/upload",
                                uploadFileHandler, false);
}