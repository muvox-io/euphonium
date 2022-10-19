#include "HTTPStream.h"

#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <ctype.h>
#include <cstring>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <netinet/tcp.h>

bell::HTTPStream::HTTPStream()
{
}

bell::HTTPStream::~HTTPStream()
{
    close();
}

void bell::HTTPStream::close()
{
    if (status != StreamStatus::CLOSED)
    {
        status = StreamStatus::CLOSED;
        BELL_LOG(info, "httpStream", "Closing socket");
        socket->close();
        BELL_LOG(info, "httpStream", "Closed socket");
    }
}

void bell::HTTPStream::connectToUrl(std::string url, bool disableSSL)
{
    std::string portString;
    // check if url contains "https"
    if (url.find("https") != std::string::npos && !disableSSL)
    {
        socket = std::make_unique<bell::TLSSocket>();
        portString = "443";
    }
    else
    {
        socket = std::make_unique<bell::TCPSocket>();
        portString = "80";
    }

    socket->open(url);

    // remove https or http from url
    url.erase(0, url.find("://") + 3);

    // split by first "/" in url
    std::string hostUrl = url.substr(0, url.find('/'));
    std::string pathUrl = url.substr(url.find('/'));

    // check if hostUrl contains ':'
    if (hostUrl.find(':') != std::string::npos)
    {
        // split by ':'
        std::string host = hostUrl.substr(0, hostUrl.find(':'));
        portString = hostUrl.substr(hostUrl.find(':') + 1);
        hostUrl = host;
    }

    // Prepare HTTP get header
    std::stringstream ss;
    ss << "GET " << pathUrl << " HTTP/1.1\r\n"
       << "Host: " << hostUrl << ":" << portString << "\r\n"
       << "Accept: */*\r\n"
       << "\r\n\r\n";

    std::string request = ss.str();

    // Send the request
    if (socket->write((uint8_t*)request.c_str(), request.length()) != (int)request.length())
    {
        close();
        BELL_LOG(error, "http", "Can't send request");
        throw std::runtime_error("Resolve failed");
    }

    status = StreamStatus::READING_HEADERS;
    auto buffer = std::vector<uint8_t>(128);
    auto currentLine = std::string();
    auto statusOkay = false;
    auto readingData = false;
    // Read data on socket sockFd line after line
    int nbytes;

    while (status == StreamStatus::READING_HEADERS)
    {
        nbytes = socket->read(&buffer[0], buffer.size());
        if (nbytes < 0)
        {
            BELL_LOG(error, "http", "Error reading from client");
            perror("recv");
            exit(EXIT_FAILURE);
        }
        else if (nbytes == 0)
        {
            BELL_LOG(error, "http", "Client disconnected");
            close();
        }
        else
        {
            currentLine += std::string(buffer.data(), buffer.data() + nbytes);
            while (currentLine.find("\r\n") != std::string::npos)
            {
                auto line = currentLine.substr(0, currentLine.find("\r\n"));
                currentLine = currentLine.substr(currentLine.find("\r\n") + 2, currentLine.size());
                BELL_LOG(info, "http", "Line: %s", line.c_str());

                // handle redirects:
                if (line.find("Location:") != std::string::npos)
                {
                    auto newUrl = line.substr(10);
                    BELL_LOG(info, "http", "Redirecting to %s", newUrl.c_str());

                    close();
                    return connectToUrl(newUrl);
                }
                // handle content-length
                if (line.find("Content-Length:") != std::string::npos)
                {
                    auto contentLengthStr = line.substr(16);
                    BELL_LOG(info, "http", "Content size %s", contentLengthStr.c_str());

                    // convert contentLengthStr to size_t
                    this->contentLength = std::stoi(contentLengthStr);
                    hasFixedSize = true;
                }
                else if (line.find("200 OK") != std::string::npos)
                {
                    statusOkay = true;
                }
                else if (line.size() == 0 && statusOkay)
                {
                    BELL_LOG(info, "http", "Ready to receive data!");
                    status = StreamStatus::READING_DATA;
                }
            }
        }
    }
}

size_t bell::HTTPStream::read(uint8_t *buf, size_t nbytes)
{
    if (status != StreamStatus::READING_DATA)
    {
        BELL_LOG(error, "http", "Not ready to read data");
        exit(0);
        return 0;
    }

    int nread = socket->read(buf, nbytes);
    if (nread < 0)
    {
        BELL_LOG(error, "http", "Error reading from client");
        close();

        perror("recv");
        exit(EXIT_FAILURE);
    }

    if (this->hasFixedSize)
    {
        this->currentPos += nread;
    }

    if (nread < nbytes)
    {
        return nread + read(buf + nread, nbytes - nread);
    }
    return nread;
}

size_t bell::HTTPStream::skip(size_t nbytes)
{
    return 0;
}
