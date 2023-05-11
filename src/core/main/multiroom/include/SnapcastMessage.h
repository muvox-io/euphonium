#pragma once

#include "nlohmann/json.hpp"

#include <sys/time.h>
#include <bit>
#include <chrono>
#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>
#include "BinaryStream.h"
#include "BellUtils.h"
#include "StreamUtils.h"

namespace snapcast {
constexpr size_t SNAPCAST_BASE_MESSAGE_SIZE = 26;

static uint16_t sequenceNum = 0;

/*
 * Message is a base class for all Snapcast messages.
 * It contains all serialization and deserialization methods.
 */
class BaseMessage {
 public:
  enum class Type {
    Base = 0,
    CodecHeader = 1,
    WireChunk = 2,
    ServerSettings = 3,
    Time = 4,
    Hello = 5,
    StreamTags = 6
  };

  Type type = Type::Base;

  uint16_t id = 0;
  uint16_t refersTo = 0;
  bell::tv sent;
  bell::tv received;
  uint32_t msgLength = 0;

  std::vector<std::byte> body;

  void assignReceivedTime() { received = bell::tv::now(); }

  void assignSentTime() { sent = bell::tv::now(); }

  void write(std::ostream& dst, uint32_t msgSize) {
    bell::BinaryStream bs(&dst);
    bs.setByteOrder(std::endian::little);

    bs << static_cast<uint16_t>(type);
    bs << id;
    bs << refersTo;
    bs << sent.sec;
    bs << sent.usec;
    bs << received.sec;
    bs << received.usec;
    bs << msgSize;
  }

  void read(std::istream& istr) {
    uint16_t tmpType;
    bell::BinaryStream bs(&istr);
    bs.setByteOrder(std::endian::little);

    bs >> tmpType;
    bs >> id;
    bs >> refersTo;
    bs >> sent.sec;
    bs >> sent.usec;
    bs >> received.sec;
    bs >> received.usec;
    bs >> msgLength;

    type = static_cast<Type>(tmpType);
    body = std::vector<std::byte>(msgLength);

    istr.read((char*)body.data(), body.size());
  }
};

/*
    "Arch": "x86_64",
    "ClientName": "Snapclient",
    "HostName": "my_hostname",
    "ID": "00:11:22:33:44:55",
    "Instance": 1,
    "MAC": "00:11:22:33:44:55",
    "OS": "Arch Linux",
    "SnapStreamProtocolVersion": 2,
    "Version": "0.17.1"
 */
struct HelloMessage {
  nlohmann::json body;

  void serialize(std::ostream& dst) {
    BaseMessage baseMsg;
    // Set type
    baseMsg.type = BaseMessage::Type::Hello;
    baseMsg.id = sequenceNum++;

    auto jsonString = body.dump();
    uint32_t bodySize = jsonString.size();

    // Assign current time
    baseMsg.assignSentTime();

    baseMsg.write(dst, jsonString.size() + sizeof(bodySize));

    // Serialize size
    bell::BinaryStream(&dst) << bodySize;

    // Write body
    dst << jsonString;
  }
};

struct ServerSettingsMessage {
  nlohmann::json body;

  void deserialize(BaseMessage& base) {
    BaseMessage baseMsg;

    // Pepare input stream
    bell::IMemoryStream istr(base.body.data(), base.body.size());
    bell::BinaryStream bs(&istr);

    // Read body size
    uint32_t bodySize;
    bs >> bodySize;

    // Read and parse json of body data
    std::vector<char> tempBody(bodySize);
    istr.read(tempBody.data(), bodySize);
    body = nlohmann::json::parse(tempBody.begin(), tempBody.end());
  }
};

struct TimeMessage {
  bell::tv latency;

  void serialize(std::ostream& dst) {
    BaseMessage baseMsg;

    // Set type
    baseMsg.type = BaseMessage::Type::Time;
    baseMsg.id = sequenceNum++;

    // Assign current time
    baseMsg.assignSentTime();

    baseMsg.write(dst, sizeof(int32_t) * 2);

    // Serialize message body
    bell::BinaryStream bs(&dst);

    bs << latency.sec;
    bs << latency.usec;
  }

  void deserialize(BaseMessage& base) {
    // Pepare input stream
    bell::IMemoryStream istr(base.body.data(), base.body.size());
    bell::BinaryStream bs(&istr);

    bs >> latency.sec;
    bs >> latency.usec;
  }
};

struct CodecHeaderMessage {
  std::string codecString;
  uint32_t payloadSize;
  // Codec info
  uint32_t sampleRate;
  uint16_t bits;
  uint16_t channels;

  static CodecHeaderMessage deserialize(BaseMessage& base) {
    CodecHeaderMessage msg;
    uint32_t codecSizeTmp;

    // Pepare input stream
    bell::IMemoryStream istr(base.body.data(), base.body.size());
    bell::BinaryStream bs(&istr);

    // Deserialize
    bs >> codecSizeTmp;
    msg.codecString = std::string(codecSizeTmp, ' ');
    istr.read(msg.codecString.data(), codecSizeTmp);
    bs >> msg.payloadSize;

    if (msg.codecString == "opus") {
      uint32_t dummy;
      bs >> dummy;
      bs >> msg.sampleRate;
      bs >> msg.bits;
      bs >> msg.channels;
    }

    return msg;
  }
};

struct WireChunkMessage {
  bell::tv timestamp;
  uint32_t dataLen;
  uint8_t* dataPtr;

  static WireChunkMessage deserialize(BaseMessage& base) {
    WireChunkMessage msg;

    // Pepare input stream
    bell::IMemoryStream istr(base.body.data(), base.body.size());
    bell::BinaryStream bs(&istr);

    // Deserialize
    bs >> msg.timestamp.sec;
    bs >> msg.timestamp.usec;
    bs >> msg.dataLen;

    // Assign a pointer to sample data
    msg.dataPtr = (uint8_t*)base.body.data() + sizeof(msg.timestamp.sec) +
                  sizeof(msg.timestamp.usec) + sizeof(msg.dataLen);
    return msg;
  }
};

}  // namespace snapcast
