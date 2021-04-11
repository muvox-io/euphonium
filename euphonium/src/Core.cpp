#include "Core.h"
#include "DupaService.h"
#include "ConfigurationApiService.h"
#include "HTTPServer.h"
#include "SystemInfo.h"
#include <string.h>
#define SOL_ALL_SAFETIES_ON 1
#include <sol.hpp>
#include <cassert>

Core::Core()
{
}

void Core::registerChildren()
{
    this->registeredServices.push_back(std::make_shared<DupaService>(shared_from_this()));
    this->registeredServices.push_back(std::make_shared<ConfigurationApiService>(shared_from_this()));
}

void checkResult(sol::protected_function_result result)
{
    if (!result.valid())
    {
        std::cout << ((sol::error)result).what() << std::endl;
    }
}

void Core::logAvailableServices()
{
    sol::state lua;

    lua.new_enum("RequestType",
                 "GET", RequestType::GET,
                 "POST", RequestType::POST);

    sol::usertype<HTTPRequest> requestType = lua.new_usertype<HTTPRequest>("HTTPRequest", sol::constructors<HTTPRequest()>());
    requestType["body"] = &HTTPRequest::body;
    requestType["urlParams"] = &HTTPRequest::urlParams;
    requestType["connection"] = &HTTPRequest::connection;

    sol::usertype<HTTPResponse> responseType = lua.new_usertype<HTTPResponse>("HTTPResponse", sol::constructors<HTTPResponse()>());
    responseType["status"] = &HTTPResponse::status;
    responseType["body"] = &HTTPResponse::body;
    responseType["contentType"] = &HTTPResponse::contentType;

    sol::usertype<HTTPServer> serverType = lua.new_usertype<HTTPServer>("HttpServer", sol::constructors<HTTPServer(int)>());
    serverType["listen"] = &HTTPServer::listen;
    serverType["respond"] = &HTTPServer::respond;
    serverType["registerHandler"] = &HTTPServer::registerHandler;

    lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table);
    std::vector<std::string> registeredPlugins({"json", "http", "app", "cspot_plugin"});

    for (auto const &value : registeredPlugins)
    {
        checkResult(lua.script_file("../../../euphonium/lua/" + value + ".lua"));
    }

    checkResult(lua.script("app:printRegisteredPlugins()"));
}

std::shared_ptr<Service> Core::getServiceWithName(std::string &name)
{
    // for (const auto& service : this->registeredServices) {
    //     if (service->serviceName == name) {
    //         return service;
    //     }
    // }

    // return nullptr;
}