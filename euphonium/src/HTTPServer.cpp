#include "HTTPServer.h"

HTTPServer::HTTPServer(int serverPort)
{
    this->serverPort = serverPort;
    this->routes = std::map<std::string, HTTPRoute>();
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

void HTTPServer::registerHandler(RequestType requestType, std::string routeUrl, httpHandler &handler)
{

    this->routes.insert({routeUrl, HTTPRoute{
                                       .requestType = requestType,
                                       .handler = handler,
                                   }});
}

void HTTPServer::listen()
{
    printf("Starting configuration server at port %d\n", this->serverPort);

    struct addrinfo hints, *server;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(NULL, std::to_string(serverPort).c_str(), &hints, &server);

    int sockfd = socket(server->ai_family,
                        server->ai_socktype, server->ai_protocol);
    bind(sockfd, server->ai_addr, server->ai_addrlen);
    ::listen(sockfd, 10);

    struct sockaddr_storage client_addr;
    socklen_t addr_size = sizeof client_addr;

    for (;;)
    {
        int clientFd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
        std::cout << "Corn part 0: acceptanmce \n";
        if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
        {
            perror("failed to fcntl(clientFd, F_SETFL, O_NONBLOCK);");
            continue;
        };
        std::string httpMethod;
        int readBytes = 0;
        std::vector<uint8_t> bufferVec(128);

        auto currentString = std::string();

        fd_set set;
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 30000;
        FD_ZERO(&set);
        FD_SET(clientFd, &set);

        std::cout << "Corn part 1 \n";

        for (;;)
        {

            int rv = select(clientFd + 1, &set, NULL, NULL, &timeout);
        std::cout << "Corn selection: yes \n";
            if (rv == -1)
            {
                perror("select"); /* an error occured */
                break;
            }
            else if (rv == 0)
            {
                break;
            }
            else
            {
                readBytes = read(clientFd, bufferVec.data(), 128);
            }

            // Read entire response so lets yeeeet
            if (readBytes <= 0)
                break;
            currentString += std::string(bufferVec.data(), bufferVec.data() + readBytes);

            while (currentString.find("\r\n") != std::string::npos)
            {
                auto line = currentString.substr(0, currentString.find("\r\n"));
                std::cout << line << "\n";
                currentString = currentString.substr(currentString.find("\r\n") + 2, currentString.size());
                if (line.find("GET ") != std::string::npos || line.find("POST ") != std::string::npos)
                {
                    httpMethod = line;
                }
            }
        }

        auto response = findAndHandleRoute(httpMethod);

        std::stringstream stream;
        stream << "HTTP/1.1 " << response.status << " OK\r\n";
        stream << "Server: EUPHONIUM\r\n";
        stream << "Connection: close\r\n";
        stream << "Content-type: text/html\r\n";
        stream << "Content-length:" << response.body.size() << "\r\n";
        stream << "\r\n";
        stream << response.body;

        auto responseStr = stream.str();
        write(clientFd, responseStr.c_str(), responseStr.size());
        shutdown(clientFd, SHUT_RDWR);
        close(clientFd);
    }
}

HTTPResponse HTTPServer::findAndHandleRoute(std::string &url)
{

    std::map<std::string, std::string> pathParams;
    for (const auto &route : this->routes)
    {
        std::string path = url;
        if (url.find("GET ") != std::string::npos)
        {
            path = path.substr(4);
        }
        else if (url.find("GET ") != std::string::npos && route.second.requestType == RequestType::POST)
        {
            path = path.substr(5);
        }
        else
        {
            continue;
        }

        path = path.substr(0, path.find(" "));

        std::cout << path << std::endl;
        std::cout << route.first << std::endl;

        auto routeSplit = splitUrl(route.first);
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
            printf("dupsko\n");
            HTTPRequest req = {
                .urlParams = pathParams,
            };
            return route.second.handler(req);
        }
    }

    return HTTPResponse{
        .status = 404,
        .body = "Not found",
    };
}