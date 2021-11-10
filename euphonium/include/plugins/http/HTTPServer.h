#ifndef EUPHONIUM_HTTP_SERVER_H
#define EUPHONIUM_HTTP_SERVER_H

#include <functional>
#include <map>
#include <optional>
#include <memory>
#include <regex>
#include <optional>
#include <set>
#include <iostream>
#include <queue>
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <EuphoniumLog.h>
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
    int handlerId;
    int connection;
};

struct HTTPResponse {
    int connectionFd;
    int status;
    std::string body;
    std::string contentType;
};

typedef std::function<void(HTTPRequest&)> httpHandler;
struct HTTPRoute {
    RequestType requestType;
    httpHandler handler;
};

struct HTTPConnection {
    std::vector<uint8_t> buffer;
    std::string currentLine = "";
    int contentLength;
    bool isReadingBody = false;
    std::string httpMethod;
};

class HTTPServer {
private:
    std::regex routerPattern = std::regex(":([^\\/]+)?");
    int serverPort;
    fd_set master;
    fd_set readFds;
    int pipeFd[2];
    
    std::queue<HTTPResponse> responseQueue;
    fd_set activeFdSet, readFdSet;
    bool isClosed = true;
    bool writingResponse = false;
    std::map<std::string, std::vector<HTTPRoute>> routes;
    std::map<int, HTTPConnection> connections;
    void writeResponse(const HTTPResponse&);
    void findAndHandleRoute(std::string&, std::string&, int connectionFd);
    std::vector<std::string> splitUrl(const std::string& url);
    void readFromClient(int clientFd);

public:
    HTTPServer(int serverPort);
    void registerHandler(RequestType requestType, const std::string&, httpHandler);
    void respond(const HTTPResponse&);
    void closeConnection(int connection);
    void listen();
};

#endif


