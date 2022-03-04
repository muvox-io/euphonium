#ifndef SNAPCAST_CONNECTION_H
#define SNAPCAST_CONNECTION_H

#include <string>
#include <vector>
#include <BellSocket.h>
#include <TCPSocket.h>
#include <EuphoniumLog.h>
#include "SnapcastMessage.h"
#include <JSONObject.h>
#include <OPUSDecoder.h>

typedef std::function<void(uint32_t, int16_t)> snapcastOutputConfigCallback;
typedef std::function<void(uint8_t*, size_t)> snapcastDataCallback;


namespace Snapcast {
class Connection {
  private:
    std::unique_ptr<bell::Socket> socket;
    std::unique_ptr<OPUSDecoder> decoder;
    std::vector<uint8_t> buffer = std::vector<uint8_t>(1024);


  public:
    snapcastOutputConfigCallback outputConfigCallback;
    snapcastDataCallback dataCallback;

    Connection();
    void writeHello();
    void connectWithServer(const std::string url);
    void handleUpdate();
};
}

#endif