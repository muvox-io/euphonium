#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "BellUtils.h"
#include "EuphContext.h"
#include "OPUSDecoder.h"
#include "SnapcastMessage.h"
#include "SocketStream.h"

namespace snapcast {
class Connection {

 public:
  Connection();
  void writeHello();
  void connectWithServer(const std::string url, int port);
  void handleUpdate(std::shared_ptr<euph::Context> ctx);

 private:
  bell::tv lastSync;
  bell::tv timeDiff;

  int16_t sequenceNum = 0;
  bell::SocketStream socketStream;
  bell::OPUSDecoder opusDecoder;
  uint32_t decodedLen;
  BaseMessage message;

  CodecHeaderMessage codecHeader;
  TimeMessage timeMessage;
};
}  // namespace snapcast
