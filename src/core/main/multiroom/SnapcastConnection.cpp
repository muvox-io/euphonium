#include "SnapcastConnection.h"

using namespace snapcast;

Connection::Connection() {}

void Connection::writeHello() {
  snapcast::HelloMessage message;
  message.body = {{"Arch", "x86_64"},   {"ClientName", "Snapclient"},
                  {"HostName", "dupa"}, {"ID", "00:11:22:33:44:55"},
                  {"Instance", 1},      {"MAC", "00:11:22:33:44:55"},
                  {"OS", "Arch Linux"}, {"SnapStreamProtocolVersion", 2},
                  {"Version", "0.17.1"}};

  message.serialize(socketStream);
  socketStream.flush();
}

void Connection::connectWithServer(const std::string url, int port) {
  this->socketStream.open(url, port, false);
  writeHello();
}

void Connection::handleUpdate(std::shared_ptr<euph::Context> ctx) {

  message.read(this->socketStream);

  if (message.type != BaseMessage::Type::WireChunk) {
    BELL_LOG(info, "snapcast", "Received message of type %d and length %d",
             message.type, message.msgLength);
  }
  message.assignReceivedTime();
  switch (message.type) {
    case BaseMessage::Type::CodecHeader: {
      BELL_LOG(info, "snapcast", "Received codec header");
      codecHeader = CodecHeaderMessage::deserialize(message);
      if (codecHeader.codecString == "opus") {
        opusDecoder.setup(48000, 2, 16);
      }

      break;
    }
    case BaseMessage::Type::ServerSettings: {
      ServerSettingsMessage serverSettings;
      serverSettings.deserialize(message);
      std::cout << serverSettings.body.dump() << std::endl;
      break;
    }
    case BaseMessage::Type::Time: {
      timeMessage.deserialize(message);
      timeDiff = bell::tv::now() - message.sent;
      /*
      int64_t timeDiffMS = (timeMessage.latency.ms() - s2c.ms()) / 2;
      timeDiff = bell::tv(std::floor(timeDiffMS / 1000.0), (int32_t)((int64_t)(timeDiffMS * 1000) % 1000000));
        */
      break;
    }

    case BaseMessage::Type::WireChunk: {
      auto wireChunk = WireChunkMessage::deserialize(message);

      // [timestamp] + [server/local diff] + [buffer_ms]
      bell::tv localTime =
          (wireChunk.timestamp + this->timeDiff) + bell::tv(1, 0);
      uint8_t* samplePointer =
          opusDecoder.decode(wireChunk.dataPtr, wireChunk.dataLen, decodedLen);

      if (samplePointer && decodedLen > 0) {
        size_t toWrite = decodedLen;
        while (toWrite > 0) {
          toWrite -= ctx->audioBuffer->writePCM(
              samplePointer + decodedLen - toWrite, toWrite, 0, 48000, 2,
              bell::BitWidth::BW_16, localTime.sec, localTime.usec);
        }
      }
    }
    default:
      break;
  }

  bell::tv timeDelta = bell::tv::now() - lastSync;

  // Second passed since last sync
  if (timeDelta.sec >= 1) {
    lastSync = bell::tv::now();
    timeMessage.latency = lastSync;
    timeMessage.serialize(socketStream);
    socketStream.flush();
  }
}
