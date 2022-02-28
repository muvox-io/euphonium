#ifndef SNAPCAST_CONNECTION_H
#define SNAPCAST_CONNECTION_H

#include <string>
#include <vector>
#include <BellSocket.h>
#include <TCPSocket.h>
#include <EuphoniumLog.h>
#include "SnapcastMessage.h"
#include <JSONObject.h>

class SnapcastConnection
{
  private:
    std::unique_ptr<bell::Socket> socket;
  public:
    SnapcastConnection();
    void writeHello();
    void connectWithServer(const std::string url);
};

#endif