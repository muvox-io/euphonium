#include "HTTPModule.h"
#include "HTTPEvents.h"

HTTPModule::HTTPModule() {
    name = "http";
    server = std::make_shared<HTTPServer>(2137);
}

void HTTPModule::loadScript(std::shared_ptr<ScriptLoader> scriptLoader) {
    scriptLoader->loadScript("http", luaState);
}

void HTTPModule::setupLuaBindings() {
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

void HTTPModule::registerHandler(const std::string &routeUrl, RequestType reqType, int handlerId) {
    auto handler = [handlerId, this](HTTPRequest& request) {
        request.handlerId = handlerId;
        auto event = std::make_unique<HandleRouteEvent>(request);
        this->luaEventBus->postEvent(std::move(event));
    };
    
    std::cout << "Rehistering handler for " << routeUrl << std::endl;
    server->registerHandler(reqType, routeUrl, handler);
}

void HTTPModule::listen() {
    server->listen();
}

void HTTPModule::startAudioThread() {
    std::thread newThread(&HTTPModule::listen, this);
    newThread.detach();
}