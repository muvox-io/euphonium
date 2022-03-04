//
// Created by Filip Grzywok on 28/02/2022.
//

#ifndef EUPHONIUMCLI_SNAPCASTMESSAGE_H
#define EUPHONIUMCLI_SNAPCASTMESSAGE_H

#ifdef IS_BIG_ENDIAN
#define SWAP_16(x) (__builtin_bswap16(x))
#define SWAP_32(x) (__builtin_bswap32(x))
#define SWAP_64(x) (__builtin_bswap64(x))
#else
#define SWAP_16(x) x
#define SWAP_32(x) x
#define SWAP_64(x) x
#endif

#include <vector>
#include <JSONObject.h>
#include <memory>
#include <chrono>
#include <iostream>
#include <sys/time.h>

#define SNAPCAST_BASE_MESSAGE_SIZE 26

namespace Snapcast {

struct tv
{
    tv()
    {
        timeval t;
        gettimeofday(&t, NULL);
        sec = t.tv_sec;
        usec = t.tv_usec;
    }
    tv(timeval tv) : sec(tv.tv_sec), usec(tv.tv_usec){};
    tv(int32_t _sec, int32_t _usec) : sec(_sec), usec(_usec){};

    int32_t sec;
    int32_t usec;

    tv operator+(const tv& other) const
    {
        tv result(*this);
        result.sec += other.sec;
        result.usec += other.usec;
        if (result.usec > 1000000)
        {
            result.sec += result.usec / 1000000;
            result.usec %= 1000000;
        }
        return result;
    }

    tv operator-(const tv& other) const
    {
        tv result(*this);
        result.sec -= other.sec;
        result.usec -= other.usec;
        while (result.usec < 0)
        {
            result.sec -= 1;
            result.usec += 1000000;
        }
        return result;
    }
};

/*
 * Message is a base class for all Snapcast messages.
 * It contains all serialization and deserialization methods.
 */
class Message {
  public:
    enum class MessageType {
        Base = 0,
        CodecHeader = 1,
        WireChunk = 2,
        ServerSettings = 3,
        Time = 4,
        Hello = 5,
        StreamTags = 6
    };

    std::vector<uint8_t> buffer;
    MessageType type;
    uint16_t id;
    uint16_t refersTo;
    int32_t sentSec;
    int32_t sentUsec;
    int32_t receivedSec;
    int32_t receivedUsec;
    uint32_t msgLength;

    Message() {};

//    virtual std::vector<uint8_t> serialize() = 0;

    void writeBaseMessage(uint32_t msgSize) {
        writeVal(static_cast<uint16_t>(type));
        writeVal(id);
        writeVal(refersTo);
        writeVal(sentSec);
        writeVal(sentUsec);
        writeVal(receivedSec);
        writeVal(receivedUsec);
        writeVal(msgSize);
    }

    void readBaseMessage(uint8_t* buffer) {
        uint16_t tmpType;
        auto readPos = 0;
        readPos += readVal(buffer+ readPos, tmpType);
        type = static_cast<MessageType>(tmpType);
        readPos += readVal(buffer + readPos, id);
        readPos += readVal(buffer + readPos, refersTo);
        readPos += readVal(buffer + readPos, sentSec);
        readPos += readVal(buffer + readPos, sentUsec);
        readPos += readVal(buffer + readPos, receivedSec);
        readPos += readVal(buffer + readPos, receivedUsec);
        readVal(buffer + readPos, msgLength);
    }

    void writeVal(const uint8_t val) { this->buffer.push_back(val); }

    void writeVal(const uint16_t &val) {
        uint16_t v = SWAP_16(val);
        auto castVal = reinterpret_cast<const uint8_t *>(&v);
        buffer.insert(buffer.end(), castVal, castVal + sizeof(uint16_t));
    }

    void writeVal(const int16_t &val) {
        uint16_t v = SWAP_16(val);
        auto castVal = reinterpret_cast<const uint8_t *>(&v);
        buffer.insert(buffer.end(), castVal, castVal + sizeof(int16_t));
    }

    void writeVal(const uint32_t &val) {
        uint32_t v = SWAP_32(val);
        auto castVal = reinterpret_cast<const uint8_t *>(&v);
        buffer.insert(buffer.end(), castVal, castVal + sizeof(uint32_t));
    }

    void writeVal(const int32_t &val) {
        int32_t v = SWAP_32(val);
        auto castVal = reinterpret_cast<const int8_t *>(&v);
        buffer.insert(buffer.end(), castVal, castVal + sizeof(int32_t));
    }

    size_t readVal(uint8_t* stream, char& val) const
    {
        val = stream[0];
        return 1;
    }

    size_t readVal(uint8_t* stream, uint16_t& val) const
    {
        val = SWAP_16(*reinterpret_cast<uint16_t*>(stream));
        return 2;
    }

    size_t readVal(uint8_t* stream, int16_t& val) const
    {
        val = SWAP_16(*reinterpret_cast<int16_t*>(stream));
        return 2;
    }


    size_t readVal(uint8_t* stream, uint32_t& val) const
    {
        val = SWAP_32(*reinterpret_cast<uint32_t*>(stream));
        return 4;
    }


    size_t readVal(uint8_t* stream, int32_t& val) const
    {
        val = SWAP_32(*reinterpret_cast<int32_t*>(stream));
        return 4;
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
class HelloMessage {
  private:
    bell::JSONObject json;
    Message baseMessage;
  public:
    HelloMessage() {
        baseMessage.type = Message::MessageType::Hello;
        baseMessage.id = 0;
        baseMessage.refersTo = 0;
        baseMessage.sentSec = 0;
        baseMessage.sentUsec = 0;
        baseMessage.receivedSec = 0;

    };

    void setData(const std::string &arch, const std::string& clientName, const std::string& hostName, const std::string& id, int instance, const std::string& mac, const std::string& os, int snapcastProtocolVersion, const std::string& version) {
        json["Arch"] = arch;
        json["ClientName"] = clientName;
        json["HostName"] = hostName;
        json["ID"] = id;
        json["Instance"] = instance;
        json["MAC"] = mac;
        json["OS"] = os;
        json["SnapStreamProtocolVersion"] = snapcastProtocolVersion;
        json["Version"] = version;
    }
    std::vector<uint8_t> serialize() {
        auto time = tv();
        baseMessage.sentSec = time.sec;
        baseMessage.sentUsec = time.usec;

        auto jsonData = json.toVector();

        baseMessage.writeBaseMessage(jsonData.size() + 4);
        baseMessage.writeVal((uint32_t) jsonData.size());
        baseMessage.buffer.insert(baseMessage.buffer.end(), jsonData.begin(), jsonData.end());
        return baseMessage.buffer;
    }
};

#define SNAPCAST_CODEC_FLAC "flac"

class CodecHeaderMessage {
  private:
    Message baseMessage;

  public:
    std::string codecString;
    uint32_t payloadSize;
    uint8_t* payloadPtr;

    // Codec info
    uint32_t sampleRate;
    uint16_t bits;
    uint16_t channels;



    CodecHeaderMessage() {
        baseMessage.type = Message::MessageType::CodecHeader;
        baseMessage.id = 0;
        baseMessage.refersTo = 0;
        baseMessage.sentSec = 0;
        baseMessage.sentUsec = 0;
        baseMessage.receivedSec = 0;
    };

    void deserialize(std::vector<uint8_t>& buffer) {
        auto readSize = 0;

        uint32_t codecSize;
        readSize += baseMessage.readVal(buffer.data(), codecSize);
        codecString = std::string(buffer.data() + readSize, buffer.data() + readSize + codecSize);
        readSize += codecSize;
        readSize += baseMessage.readVal(buffer.data() + readSize, payloadSize);
        payloadPtr = buffer.data() + readSize;

        if (codecString == "opus") {
            // Decode base opus header
            baseMessage.readVal(payloadPtr + 4, sampleRate);
            baseMessage.readVal(payloadPtr + 8, bits);
            baseMessage.readVal(payloadPtr + 10, channels);
        }
    }
};
}

#endif // EUPHONIUMCLI_SNAPCASTMESSAGE_H