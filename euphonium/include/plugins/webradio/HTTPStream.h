#ifndef EUPHONIUM_HTTP_STREAM_H
#define EUPHONIUM_HTTP_STREAM_H

#include <string>
#include <EuphoniumLog.h>

enum class StreamStatus {
    OPENING,
    READING_HEADERS,
    READING_DATA,
    CLOSED
};

class HTTPStream {
    public:
    HTTPStream();
    ~HTTPStream();

    int sockFd;

    StreamStatus status = StreamStatus::OPENING;
    void connectToUrl(std::string url);

    size_t read(uint8_t *buf, size_t nbytes);
    size_t skip(size_t nbytes);
    void close();
};
#endif