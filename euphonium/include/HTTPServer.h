
#ifndef EUPHONIUM_HTTP_SERVER_H
#define EUPHONIUM_HTTP_SERVER_H

#include <functional>
#include <map>
#include <optional>
#include <memory>
#include <regex>
#include <optional>
#include "Core.h"
#include "Service.h"
#include "protobuf.h"

#include <sstream>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string>
#include <netdb.h>
#include <fcntl.h>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#ifndef SOCK_NONBLOCK
#define SOCK_NONBLOCK O_NONBLOCK
#endif


enum class RequestType {
    GET,
    POST
};

struct HTTPRequest {
    std::map<std::string, std::string> urlParams;
    std::string body;
    int connection;
};

struct HTTPResponse {
    int status;
    std::string body;
    std::string contentType;
};

typedef std::function<void(const HTTPRequest&)> httpHandler;

struct HTTPRoute {
    RequestType requestType;
    httpHandler handler;
};

class HTTPServer {
private:
    std::regex routerPattern = std::regex(":([^\\/]+)?");
    int serverPort;
    fd_set master;
    fd_set readFds;
    std::map<std::string, HTTPRoute> routes;
    void findAndHandleRoute(std::string&, int connectionFd);
    std::vector<std::string> splitUrl(const std::string& url);

public:
    HTTPServer(int serverPort);
    void registerHandler(RequestType requestType, const std::string&, httpHandler);
    void respond(const HTTPResponse&, int connection);
    void listen();
};

#endif


