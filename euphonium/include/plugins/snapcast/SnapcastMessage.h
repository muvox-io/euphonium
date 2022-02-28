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

    Message() {};

    virtual std::vector<uint8_t> serialize() = 0;
/*
 * result |= buffer_write_uint16(&buffer, msg->type);
    result |= buffer_write_uint16(&buffer, msg->id);
    result |= buffer_write_uint16(&buffer, msg->refersTo);
    result |= buffer_write_int32(&buffer, msg->sent.sec);
    result |= buffer_write_int32(&buffer, msg->sent.usec);
    result |= buffer_write_int32(&buffer, msg->received.sec);
    result |= buffer_write_int32(&buffer, msg->received.usec);
    result |= buffer_write_uint32(&buffer, msg->size);
 */
    void writeBaseMessage(uint16_t type, uint16_t id, uint16_t refersTo, int32_t sentSec, int32_t sentUsec, int32_t receivedSec, int32_t receivedUsec, uint32_t msgSize) {
        writeVal(type);
        writeVal(id);
        writeVal(refersTo);
        writeVal(sentSec);
        writeVal(sentUsec);
        writeVal(receivedSec);
        writeVal(receivedUsec);
        writeVal(msgSize);
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
class HelloMessage : public Message {
  private:
    bell::JSONObject json;
  public:
    HelloMessage() {};

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
    std::vector<uint8_t> serialize() override {
        auto time = tv();
        writeBaseMessage(static_cast<uint16_t>(MessageType::Hello), 0, 0, time.sec, time.usec, 0, 0, 26);
        auto jsonData = json.toVector();
        writeVal((uint32_t) jsonData.size());
        buffer.insert(buffer.end(), jsonData.begin(), jsonData.end());
        return buffer;
    }
};
}

#endif // EUPHONIUMCLI_SNAPCASTMESSAGE_H