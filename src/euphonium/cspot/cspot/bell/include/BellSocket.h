#ifndef BELL_SOCKET_H
#define BELL_SOCKET_H

#include <string>

namespace bell {
    class Socket {
        public:
            Socket() {};
            virtual ~Socket() = default;

			void open(const std::string &url);
			virtual void open(std::string host, uint16_t port) = 0;
			virtual size_t poll() = 0;
            virtual size_t write(uint8_t* buf, size_t len) = 0;
            virtual size_t read(uint8_t* buf, size_t len) = 0;
            virtual void close() = 0;
    };
}

#endif
