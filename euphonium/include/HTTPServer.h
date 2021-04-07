
#ifndef EUPHONIUM_HTTP_SERVER_H
#define EUPHONIUM_HTTP_SERVER_H

#include <functional>
#include <map>
#include <optional>
#include <memory>
#include <regex>
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
#ifndef SOCK_NONBLOCK
#include <fcntl.h>
#define SOCK_NONBLOCK O_NONBLOCK
#endif


enum class RequestType {
    GET,
    POST
};

struct HTTPRequest {
    std::map<std::string, std::string> urlParams;
    std::string body;
};

struct HTTPResponse {
    int status;
    std::string body;
};

typedef std::function<HTTPResponse(HTTPRequest&)> httpHandler;

struct HTTPRoute {
    const httpHandler& handler;
    RequestType requestType;
};

class HTTPServer {
private:
    std::regex routerPattern = std::regex(":([^\\/]+)?");
    int serverPort;
    std::map<std::string, HTTPRoute> routes;
    HTTPResponse findAndHandleRoute(std::string&);
    std::vector<std::string> splitUrl(const std::string& url);

public:
    HTTPServer(int serverPort);
    void registerHandler(RequestType, std::string, httpHandler& handler);
    void listen();
};

#endif


