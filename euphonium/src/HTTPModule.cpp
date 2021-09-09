#include "HTTPModule.h"

HTTPModule::HTTPModule() {
    name = "http";
}

void HTTPModule::loadScript(std::shared_ptr<ScriptLoader> scriptLoader, std::shared_ptr<sol::state> luaState) {
    scriptLoader->loadScript("http", luaState);
}

void HTTPModule::setupLuaBindings(std::shared_ptr<sol::state> luaState) {
    luaState->new_enum("RequestType",
                 "GET", RequestType::GET,
                 "POST", RequestType::POST);

    sol::usertype<HTTPRequest> requestType = luaState->new_usertype<HTTPRequest>("HTTPRequest", sol::constructors<HTTPRequest()>());
    requestType["body"] = &HTTPRequest::body;
    requestType["urlParams"] = &HTTPRequest::urlParams;
    requestType["connection"] = &HTTPRequest::connection;

    sol::usertype<HTTPResponse> responseType = luaState->new_usertype<HTTPResponse>("HTTPResponse", sol::constructors<HTTPResponse()>());
    responseType["status"] = &HTTPResponse::status;
    responseType["body"] = &HTTPResponse::body;
    responseType["contentType"] = &HTTPResponse::contentType;

    sol::usertype<HTTPServer> serverType = luaState->new_usertype<HTTPServer>("HttpServer", sol::constructors<HTTPServer(int)>());
    serverType["listen"] = &HTTPServer::listen;
    serverType["respond"] = &HTTPServer::respond;
    serverType["registerHandler"] = &HTTPServer::registerHandler;
}