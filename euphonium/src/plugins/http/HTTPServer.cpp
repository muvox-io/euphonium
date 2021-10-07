#include "HTTPServer.h"

HTTPServer::HTTPServer(int serverPort)
{
    this->serverPort = serverPort;
}

std::vector<std::string> HTTPServer::splitUrl(const std::string &url)
{
    std::stringstream ssb(url);
    std::string segment;
    std::vector<std::string> seglist;

    while (std::getline(ssb, segment, '/'))
    {
        seglist.push_back(segment);
    }
    return seglist;
}

void HTTPServer::registerHandler(RequestType requestType, const std::string &routeUrl, httpHandler handler)
{
    if (routes.find(routeUrl) == routes.end()) {
        routes.insert({routeUrl, std::vector<HTTPRoute>()});
    }
    this->routes[routeUrl].push_back(HTTPRoute{
        .requestType = requestType,
        .handler = handler,
    });
}

// @TODO: Implement a proper http server here. It's currently a huge mess of an implementation, and will most likely
// mess something up when more than 1 clients connect at once
void HTTPServer::listen()
{
    printf("Starting configuration server at port %d\n", this->serverPort);

    // master filedescriptor

    int fdMax; // maximum current fd
    int readBytes;

    int yes = 1;                        // used for setsockopt
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;
    // stores http method + path
    std::string httpMethod;
    std::string body;
    int contentLength = 0;
    bool readingBody = false;

    // input data buffer
    std::vector<uint8_t> bufferVec(128);

    // setup address
    struct addrinfo hints, *server;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, std::to_string(serverPort).c_str(), &hints, &server);

    int sockfd = socket(server->ai_family,
                        server->ai_socktype, server->ai_protocol);

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    bind(sockfd, server->ai_addr, server->ai_addrlen);
    ::listen(sockfd, 10);

    FD_SET(sockfd, &master);
    fdMax = sockfd; // listener socket is the highest descriptor rn
    auto currentString = std::string();
    for (;;)
    {
        readFds = master; // copy it
        if (select(fdMax + 1, &readFds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for (int i = 0; i <= fdMax; i++)
        {
            if (FD_ISSET(i, &readFds))
            { // we got one!!
                if (i == sockfd)
                {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    int newFd = accept(sockfd,
                                       (struct sockaddr *)&remoteaddr,
                                       &addrlen);

                    if (newFd == -1)
                    {
                        perror("accept");
                    }
                    else
                    {
                        FD_SET(newFd, &master); // add to master set
                        if (newFd > fdMax)
                        {
                            fdMax = newFd;
                        }
                        std::cout << "New connection\n";
                        currentString = std::string();
                    }
                }
                else
                {
                    // Receive data
                    if ((readBytes = recv(i, bufferVec.data(), 128, 0)) <= 0)
                    {
                        // got error or connection closed by client
                        if (readBytes == 0)
                        {
                            // connection closed
                            std::cout << "Socket " << i << "hung up\n";
                        }
                        else
                        {
                            perror("recv");
                        }
                        close(i);
                        FD_CLR(i, &master);
                    }
                    else
                    {
                        currentString += std::string(bufferVec.data(), bufferVec.data() + readBytes);
HANDLEBODY:
                        if (readingBody && body.size() < contentLength)
                        {
                            body += currentString;
                            currentString = "";
                            if (body.size() >= contentLength)
                            {
                                readingBody = false;
                                std::cout << body << std::endl;
                                findAndHandleRoute(httpMethod, body, i);
                                contentLength = 0;
                                body = "";
                            }
                        }
                        else if (!readingBody)
                        {

                            while (currentString.find("\r\n") != std::string::npos)
                            {
                                auto line = currentString.substr(0, currentString.find("\r\n"));
                                currentString = currentString.substr(currentString.find("\r\n") + 2, currentString.size());
                                if (line.find("GET ") != std::string::npos || line.find("POST ") != std::string::npos)
                                {
                                    httpMethod = line;
                                }

                                if (line.find("Content-Length: ") != std::string::npos)
                                {
                                    contentLength = std::stoi(line.substr(16, line.size() - 1));
                                    readingBody = true;
                                    goto HANDLEBODY;
                                    break;
                                }

                                if (line.size() == 0)
                                {
                                    findAndHandleRoute(httpMethod, body, i);
                                    readingBody = false;
                                    body = "";
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void HTTPServer::respond(const HTTPResponse &response, int connectionFd)
{
    std::stringstream stream;
    stream << "HTTP/1.1 " << response.status << " OK\r\n";
    stream << "Server: EUPHONIUM\r\n";
    stream << "Connection: close\r\n";
    stream << "Content-type: " << response.contentType << "\r\n";
    stream << "Content-length:" << response.body.size() << "\r\n";
    stream << "Access-Control-Allow-Origin: *\r\n";
    stream << "\r\n";
    stream << response.body;

    auto responseStr = stream.str();
    write(connectionFd, responseStr.c_str(), responseStr.size());
    close(connectionFd);
    FD_CLR(connectionFd, &master);
    writingResponse = false;
}

void HTTPServer::findAndHandleRoute(std::string &url, std::string &body, int connectionFd)
{

    std::map<std::string, std::string> pathParams;
    for (const auto &routeSet : this->routes)
    {
        for (const auto &route : routeSet.second)
        {

            std::string path = url;
            if (url.find("GET ") != std::string::npos && route.requestType == RequestType::GET)
            {
                path = path.substr(4);
            }
            else if (url.find("POST ") != std::string::npos && route.requestType == RequestType::POST)
            {
                path = path.substr(5);
            }
            else
            {
                continue;
            }

            path = path.substr(0, path.find(" "));

            auto routeSplit = splitUrl(routeSet.first);
            auto urlSplit = splitUrl(path);
            bool matches = true;

            pathParams.clear();

            if (routeSplit.size() == urlSplit.size())
            {
                for (int x = 0; x < routeSplit.size(); x++)
                {
                    if (routeSplit[x] != urlSplit[x])
                    {
                        if (routeSplit[x][0] == ':')
                        {
                            std::cout << "Found param " << routeSplit[x].substr(1) << ": " << urlSplit[x] << std::endl;
                            pathParams.insert({routeSplit[x].substr(1), urlSplit[x]});
                        }
                        else
                        {
                            matches = false;
                        }
                    }
                }
            }
            else
            {
                matches = false;
            }

            if (matches)
            {
                HTTPRequest req = {
                    .urlParams = pathParams,
                    .body = body,
                    .handlerId = 0,
                    .connection = connectionFd};
                writingResponse = true;
                route.handler(req);
                while(writingResponse);
                return;
            }
        }
    }
    respond(HTTPResponse{
                .status = 404,
                .body = "Not found",
            },
            connectionFd);
}