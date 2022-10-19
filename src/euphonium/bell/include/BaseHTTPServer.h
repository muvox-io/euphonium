#ifndef BELL_BASE_HTTP_SERV
#define BELL_BASE_HTTP_SERV

#include <cstdio>
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <iostream>
#include <vector>
#include <sys/socket.h>

namespace bell {

class ResponseReader {
  public:
    ResponseReader(){};
    virtual ~ResponseReader() = default;

    virtual size_t getTotalSize() = 0;
    virtual size_t read(char *buffer, size_t size) = 0;
    virtual void close() = 0;
};

class RequestBodyReader : public ResponseReader {
  public:
    std::vector<uint8_t> partialBuffer;
    int fd = 0;
    size_t contentLength = 0;
    size_t sizeRead = 0;

    RequestBodyReader(size_t contentLength, int fd, std::vector<uint8_t> &partialBuffer) {
        this->contentLength = contentLength;
        this->partialBuffer = partialBuffer;
        this->fd = fd;
    };


    size_t read(char *buffer, size_t size) {
        if (sizeRead < partialBuffer.size()) {
            size_t toRead = std::min(size, partialBuffer.size() - sizeRead);
            memcpy(buffer, &partialBuffer[sizeRead], toRead);
            sizeRead += toRead;
            return toRead;
        } else {
            size_t toRead = std::min(size, contentLength - sizeRead);
            size_t read = recv(fd, buffer, toRead, 0);
            sizeRead += read;
            return read;
        }
    }

    void close() {
    }

    size_t getTotalSize() { return contentLength; }
};

class FileResponseReader : public ResponseReader {
  public:
    FILE *file;
    size_t fileSize;
    FileResponseReader(std::string fileName) {
        file = fopen(fileName.c_str(), "r");
        fseek(file, 0, SEEK_END); // seek to end of file
        fileSize = ftell(file);   // get current file pointer
        fseek(file, 0, SEEK_SET); // seek back to beginning of file
    };


    size_t read(char *buffer, size_t size) {
        return fread(buffer, 1, size, file);
    }

    void close() {
        fclose(file);
    }

    size_t getTotalSize() { return fileSize; }
};

enum class RequestType { GET, POST };

struct HTTPRequest {
    std::map<std::string, std::string> urlParams;
    std::map<std::string, std::string> queryParams;
    std::unique_ptr<ResponseReader> responseReader = std::unique_ptr<RequestBodyReader>(nullptr);

    std::string body;
    std::string url;
    int handlerId;
    int connection;
    int contentLength;
};

struct HTTPResponse {
    int connectionFd;
    int status;
    bool useGzip = false;
    std::string body;
    std::string contentType;
    std::vector<std::string> extraHeaders = std::vector<std::string>();
    std::unique_ptr<ResponseReader> responseReader;
};

typedef std::function<void(std::unique_ptr<bell::HTTPRequest>)> httpHandler;

struct HTTPRoute {
    RequestType requestType;
    httpHandler handler;
    bool readBodyToStr;
};

struct HTTPConnection {
    int fd = 0;
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> partialBuffer = std::vector<uint8_t>();
    int contentLength = 0;
    std::string httpMethod;
    bool toBeClosed = false;
    bool headersRead = false;
    bool isEventConnection = false;
    bool isCaptivePortal = false;
};

class BaseHTTPServer {
public:
    BaseHTTPServer() {};
    virtual ~BaseHTTPServer() = default;

    /**
     * Should contain server's bind port
     */
    int serverPort;

    /**
     * Called when handler is being registered on the http server
     *
     * @param requestType GET or POST
     * @param endpoint registering under
     * @param readResponseToStr if true, response will be read to string, otherwise it will return a reader object
     * httpHandler lambda to be called when given endpoint gets executed
     */
    virtual void registerHandler(RequestType requestType, const std::string & endpoint,
                                 httpHandler, bool readResponseToStr = true) = 0;

    /**
     * Writes given response to a fd
     */
    virtual void respond(const HTTPResponse &) = 0;
};
} // namespace bell

#endif
