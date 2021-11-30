#include "HTTPModule.h"
#include "HTTPEvents.h"

std::shared_ptr<bell::HTTPServer> mainServer;

HTTPModule::HTTPModule() : bell::Task("http", 1024 * 4, 0, false)
{
    name = "http";
    mainServer = std::make_shared<bell::HTTPServer>(80);
}

void HTTPModule::loadScript(std::shared_ptr<ScriptLoader> loader)
{
    loader->loadScript("http", berry);
    this->scriptLoader = loader;
}

void HTTPModule::setupBindings()
{
    BELL_LOG(info, "http", "Registering handlers");
    berry->export_this("httpRegisterHandler", this, &HTTPModule::registerHandler);
    berry->export_this("httpRespond", this, &HTTPModule::respond);
    berry->export_this("httpPublishEvent", this, &HTTPModule::publishEvent);
}

void HTTPModule::publishEvent(std::string event, std::string data)
{
    mainServer->publishEvent(event, data);
}

void HTTPModule::respond(int connectionFd, int status, std::string body, std::string contentType)
{
    BELL_LOG(info, "http", "Responding to connection");
    BELL_LOG(info, "http", "Status: %d", status);
    BELL_LOG(info, "http", "Body: %s", body.c_str());
    bell::HTTPResponse response = {
        .status = status,
        .body = body,
        .contentType = contentType,
        .connectionFd = connectionFd};

    mainServer->respond(response);
}

void HTTPModule::registerHandler(std::string routeUrl, std::string requestType, int handlerId)
{
    auto handler = [handlerId, this](bell::HTTPRequest &request)
    {
        request.handlerId = handlerId;
        auto event = std::make_unique<HandleRouteEvent>(request);
        EUPH_LOG(info, "http", "Posting the event");
        this->luaEventBus->postEvent(std::move(event));
    };

    EUPH_LOG(debug, "http", "Registering handler for %s", routeUrl.c_str());
    auto reqType = bell::RequestType::GET;
    if (requestType == "POST")
    {
        reqType = bell::RequestType::POST;
    }
    mainServer->registerHandler(reqType, routeUrl, handler);
}

void HTTPModule::runTask()
{
    auto assetHandler = [this](bell::HTTPRequest &request)
    {
        auto fileName = request.urlParams.at("asset");
        auto extension = fileName.substr(fileName.size() - 3, fileName.size());
        auto contentType = "text/css";
        if (extension == ".js")
        {
            contentType = "application/javascript";
        }

        std::string prefix = "../../../web/dist";
#ifdef ESP_PLATFORM
        prefix = "/spiffs";
#endif

        bell::HTTPResponse response = {
            .connectionFd = request.connection,
            .status = 200,
            .contentType = contentType,
        };
        response.responseReader = std::make_unique<bell::FileResponseReader>(prefix + "/assets/" + fileName);
        mainServer->respond(response);
    };

    auto indexHandler = [this](bell::HTTPRequest &request)
    {
        std::string prefix = "../../../web/dist";
#ifdef ESP_PLATFORM
        prefix = "/spiffs";
#endif

        bell::HTTPResponse response = {
            .connectionFd = request.connection,
            .status = 200,
            .contentType = "text/html",
        };
        response.responseReader = std::make_unique<bell::FileResponseReader>(prefix + "/index.html");
        mainServer->respond(response);
    };
    mainServer->registerHandler(bell::RequestType::GET, "/assets/:asset", assetHandler);
    mainServer->registerHandler(bell::RequestType::GET, "/web/*", indexHandler);
    mainServer->listen();
}

void HTTPModule::startAudioThread()
{
    startTask();
}
