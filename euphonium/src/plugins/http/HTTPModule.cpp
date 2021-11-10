#include "HTTPModule.h"
#include "HTTPEvents.h"

HTTPModule::HTTPModule()
{
    name = "http";
    server = std::make_shared<HTTPServer>(2137);
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
                 "GET", RequestType::GET,
                 "POST", RequestType::POST);

    sol::usertype<HTTPRequest> requestType = lua.new_usertype<HTTPRequest>("HTTPRequest", sol::constructors<HTTPRequest()>());
    requestType["body"] = &HTTPRequest::body;
    requestType["urlParams"] = &HTTPRequest::urlParams;
    requestType["connection"] = &HTTPRequest::connection;
    requestType["handlerId"] = &HTTPRequest::handlerId;

    sol::usertype<HTTPResponse> responseType = lua.new_usertype<HTTPResponse>("HTTPResponse", sol::constructors<HTTPResponse()>());
    responseType["status"] = &HTTPResponse::status;
    responseType["body"] = &HTTPResponse::body;
    responseType["contentType"] = &HTTPResponse::contentType;
    responseType["connectionFd"] = &HTTPResponse::connectionFd;

    sol::usertype<HTTPServer> serverType = lua.new_usertype<HTTPServer>("HttpServer", sol::constructors<HTTPServer(int)>());
    serverType["respond"] = &HTTPServer::respond;

    lua.set_function("httpRegisterHandler", &HTTPModule::registerHandler, this);
    lua.set_function("httpRespond", &HTTPServer::respond, server);
}

void HTTPModule::registerHandler(const std::string &routeUrl, RequestType reqType, int handlerId)
{
    auto handler = [handlerId, this](HTTPRequest &request)
    {
        request.handlerId = handlerId;
        auto event = std::make_unique<HandleRouteEvent>(request);
        EUPH_LOG(info, "http", "Posting the event");
        this->luaEventBus->postEvent(std::move(event));
    };

    EUPH_LOG(debug, "http", "Registering handler for %s", routeUrl.c_str());
    server->registerHandler(reqType, routeUrl, handler);
}

void HTTPModule::listen()
{
    auto assetHandler = [this](HTTPRequest &request)
    {
        auto fileName = request.urlParams.at("asset");
        auto extension = fileName.substr(fileName.size() - 3, fileName.size());
        auto contentType = "text/css";
        if (extension == ".js") {
            contentType = "application/javascript";
        }

        auto indexContent = scriptLoader->loadFile("web/assets/" + fileName);
        HTTPResponse response = {
            .body = indexContent,
            .contentType = contentType,
            .connectionFd = request.connection,
            .status = 200};
        server->respond(response);
    };

    auto indexHandler = [this](HTTPRequest &request)
    {
        auto indexContent = scriptLoader->loadFile("web/index.html");

        HTTPResponse response = {
            .body = indexContent,
            .contentType = "text/html",
            .connectionFd = request.connection,
            .status = 200};
        server->respond(response);
    };
    server->registerHandler(RequestType::GET, "/assets/:asset", assetHandler);
    server->registerHandler(RequestType::GET, "/web", indexHandler);
    server->listen();
}

void HTTPModule::startAudioThread()
{
    std::thread newThread(&HTTPModule::listen, this);
    newThread.detach();
}