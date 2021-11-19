#include "HTTPModule.h"
#include "HTTPEvents.h"

std::shared_ptr<bell::HTTPServer> mainServer;

HTTPModule::HTTPModule() : bell::Task("http", 1024 * 4, 1)
{
    name = "http";
    mainServer = std::make_shared<bell::HTTPServer>(2137);
}

void HTTPModule::loadScript(std::shared_ptr<ScriptLoader> loader)
{
    loader->loadScript("http", luaState);
    this->scriptLoader = loader;
}

void HTTPModule::setupLuaBindings()
{
    sol::state_view lua(luaState);

    lua.new_enum("RequestType",
                 "GET", bell::RequestType::GET,
                 "POST", bell::RequestType::POST);

    sol::usertype<bell::HTTPRequest> requestType = lua.new_usertype<bell::HTTPRequest>("HTTPRequest", sol::constructors<bell::HTTPRequest()>());
    requestType["body"] = &bell::HTTPRequest::body;
    requestType["urlParams"] = &bell::HTTPRequest::urlParams;
    requestType["connection"] = &bell::HTTPRequest::connection;
    requestType["handlerId"] = &bell::HTTPRequest::handlerId;

    sol::usertype<bell::HTTPResponse> responseType = lua.new_usertype<bell::HTTPResponse>("HTTPResponse", sol::constructors<bell::HTTPResponse()>());
    responseType["status"] = &bell::HTTPResponse::status;
    responseType["body"] = &bell::HTTPResponse::body;
    responseType["contentType"] = &bell::HTTPResponse::contentType;
    responseType["connectionFd"] = &bell::HTTPResponse::connectionFd;
    
    lua.set_function("httpRegisterHandler", &HTTPModule::registerHandler, this);
    lua.set_function("httpRespond", &bell::HTTPServer::respond, mainServer);
}

void HTTPModule::registerHandler(const std::string &routeUrl, bell::RequestType reqType, int handlerId)
{
    auto handler = [handlerId, this](bell::HTTPRequest &request)
    {
        request.handlerId = handlerId;
        auto event = std::make_unique<HandleRouteEvent>(request);
        EUPH_LOG(info, "http", "Posting the event");
        this->luaEventBus->postEvent(std::move(event));
    };

    EUPH_LOG(debug, "http", "Registering handler for %s", routeUrl.c_str());
    mainServer->registerHandler(reqType, routeUrl, handler);
}

void HTTPModule::runTask()
{
    auto assetHandler = [this](bell::HTTPRequest &request)
    {
        auto fileName = request.urlParams.at("asset");
        auto extension = fileName.substr(fileName.size() - 3, fileName.size());
        auto contentType = "text/css";
        if (extension == ".js") {
            contentType = "application/javascript";
        }

        std::string prefix = "../../../web/dist";
        #ifdef ESP_PLATFORM
        prefix = "/spiffs";
        #endif

        auto indexContent = scriptLoader->loadFile(prefix + "/assets/" + fileName);
        bell::HTTPResponse response = {
            .connectionFd = request.connection,
            .status = 200,
            .body = indexContent,
            .contentType = contentType,
        };
        mainServer->respond(response);
    };

    auto indexHandler = [this](bell::HTTPRequest &request)
    {
        std::string prefix = "../../../web/dist";
        #ifdef ESP_PLATFORM
        prefix = "/spiffs";
        #endif

        auto indexContent = scriptLoader->loadFile(prefix + "/index.html");

        bell::HTTPResponse response = {
            .connectionFd = request.connection,
            .status = 200,
            .body = indexContent,
            .contentType = "text/html",
    };
        mainServer->respond(response);
    };
    mainServer->registerHandler(bell::RequestType::GET, "/assets/:asset", assetHandler);
    mainServer->registerHandler(bell::RequestType::GET, "/web", indexHandler);
    mainServer->listen();
}

void HTTPModule::startAudioThread()
{
    startTask();
}