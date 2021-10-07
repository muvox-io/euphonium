#include "HTTPModule.h"
#include "HTTPEvents.h"

HTTPModule::HTTPModule()
{
    name = "http";
    server = std::make_shared<HTTPServer>(2137);
}

void HTTPModule::loadScript(std::shared_ptr<ScriptLoader> scriptLoader)
{
    scriptLoader->loadScript("http", luaState);
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
        this->luaEventBus->postEvent(std::move(event));
    };

    std::cout << "Registering handler for " << routeUrl << std::endl;
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

        std::ifstream indexFile("web/assets/" + fileName);
        std::string indexContent((std::istreambuf_iterator<char>(indexFile)),
                               std::istreambuf_iterator<char>());
        HTTPResponse response = {
            .body = indexContent,
            .contentType = contentType,
            .status = 200};
        server->respond(response, request.connection);
    };

    auto indexHandler = [this](HTTPRequest &request)
    {
        std::ifstream indexFile("web/index.html");
        std::string indexContent((std::istreambuf_iterator<char>(indexFile)),
                               std::istreambuf_iterator<char>());

        HTTPResponse response = {
            .body = indexContent,
            .contentType = "text/html",
            .status = 200};
        server->respond(response, request.connection);
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