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
    EUPH_LOG(info, "http", routeUrl.c_str());
    if (routes.find(routeUrl) == routes.end())
    {
        routes.insert({routeUrl, std::vector<HTTPRoute>()});
    }
    this->routes[routeUrl].push_back(HTTPRoute{
        .requestType = requestType,
        .handler = handler,
    });
}

void HTTPServer::listen()
{
    EUPH_LOG(info, "http", "Starting configuration server at port %d", this->serverPort);

    int fdMax;
    socklen_t addrlen;

    // setup address
    struct addrinfo hints, *server;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, std::to_string(serverPort).c_str(), &hints, &server);

    int sockfd = socket(server->ai_family,
                        server->ai_socktype, server->ai_protocol);
    pipe(pipeFd);
    struct sockaddr_in clientname;
    socklen_t incomingSockSize;
    int i;
    int yes = true;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    bind(sockfd, server->ai_addr, server->ai_addrlen);
    ::listen(sockfd, 10);

    FD_ZERO(&activeFdSet);
    FD_SET(sockfd, &activeFdSet);
    FD_SET(pipeFd[0], &activeFdSet);

    for (;;)
    {
        /* Block until input arrives on one or more active sockets. */
        readFdSet = activeFdSet;
        if (select(FD_SETSIZE, &readFdSet, NULL, NULL, NULL) < 0)
        {
            EUPH_LOG(error, "http", "Error in select");
            perror("select");
            exit(EXIT_FAILURE);
        }

        /* Service all the sockets with input pending. */
        for (i = 0; i < FD_SETSIZE; ++i)
            if (FD_ISSET(i, &readFdSet))
            {
                if (i == sockfd)
                {
                    /* Connection request on original socket. */
                    int newFd;
                    incomingSockSize = sizeof(clientname);
                    newFd = accept(sockfd, (struct sockaddr *)&clientname, &incomingSockSize);
                    if (newFd < 0)
                    {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }

                    FD_SET(newFd, &activeFdSet);

                    HTTPConnection conn = {
                        .buffer = std::vector<uint8_t>(128)};

                    this->connections.insert({newFd, conn});
                }
                else if (i == pipeFd[0])
                {
                    EUPH_LOG(info, "http", "Pipe has data");
                    char dummy;
                    read(pipeFd[0], &dummy, 1);

                    if (responseQueue.size() > 0)
                    {
                        auto response = responseQueue.front();
                        responseQueue.pop();
                        writeResponse(response);
                    }
                }
                else
                {
                    /* Data arriving on an already-connected socket. */
                    readFromClient(i);
                }
            }
    }
}

void HTTPServer::readFromClient(int clientFd)
{
    HTTPConnection &conn = this->connections[clientFd];

    int nbytes = recv(clientFd, &conn.buffer[0], conn.buffer.size(), 0);
    if (nbytes < 0)
    {
        EUPH_LOG(error, "http", "Error reading from client");
        perror("recv");
        exit(EXIT_FAILURE);
    }
    else if (nbytes == 0)
    {
        EUPH_LOG(info, "http", "Read the body");
        this->closeConnection(clientFd);
    }
    else
    {
        conn.currentLine += std::string(conn.buffer.data(), conn.buffer.data() + nbytes);
        if (!conn.isReadingBody)
        {
            while (conn.currentLine.find("\r\n") != std::string::npos)
            {
                auto line = conn.currentLine.substr(0, conn.currentLine.find("\r\n"));
                conn.currentLine = conn.currentLine.substr(conn.currentLine.find("\r\n") + 2, conn.currentLine.size());
                if (line.find("GET ") != std::string::npos || line.find("POST ") != std::string::npos)
                {
                    EUPH_LOG(info, "http", "Found method line request");
                    conn.httpMethod = line;
                }

                if (line.find("Content-Length: ") != std::string::npos)
                {
                    EUPH_LOG(info, "http", "Found Content-Length");
                    conn.contentLength = std::stoi(line.substr(16, line.size() - 1));
                    conn.isReadingBody = true;
                }
                if (line.size() == 0)
                {
                    EUPH_LOG(info, "http", "Found end of headers");
                    EUPH_LOG(info, "http", conn.httpMethod.c_str());
                    findAndHandleRoute(conn.httpMethod, conn.currentLine, clientFd);
                }
            }
        }
        else
        {
            if (conn.currentLine.size() == conn.contentLength)
            {
                EUPH_LOG(info, "http", "Read the body");
                findAndHandleRoute(conn.httpMethod, conn.currentLine, clientFd);
            }
        }
    }
}

void HTTPServer::closeConnection(int connection)
{
    close(connection);
    FD_CLR(connection, &activeFdSet);
    this->connections.erase(connection);
}

void HTTPServer::writeResponse(const HTTPResponse &response)
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
    write(response.connectionFd, responseStr.c_str(), responseStr.size());
    this->closeConnection(response.connectionFd);
}

void HTTPServer::respond(const HTTPResponse &response)
{
    // add response to response queue
    responseQueue.push(response);

    // write one byte to pipeFd[1]
    int dummy = 1;
    write(pipeFd[1], &dummy, 1);
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

                route.handler(req);
                return;
            }
        }
    }
    writeResponse(HTTPResponse{
                .status = 404,
                .body = "Not found",
                .connectionFd = connectionFd
            });
}