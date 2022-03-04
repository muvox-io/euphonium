#include "SnapcastConnection.h"
#include "BellUtils.h"

using namespace Snapcast;

Connection::Connection() {
    this->socket = std::make_unique<bell::TCPSocket>();
    this->decoder = std::make_unique<OPUSDecoder>();
}

void Connection::writeHello() {
    auto message = Snapcast::HelloMessage();
    message.setData("x86_64", "Euphonium", "euphonium", "00:11:22:33:44:55", 1, "00:11:22:33:44:55", "Arch Linux", 2, "0.0.1");
    auto serialized = message.serialize();

    this->socket->write(serialized.data(), serialized.size());
}

void Connection::connectWithServer(const std::string url) {
    socket->open("192.168.1.18", 1704);
    writeHello();
}

void Connection::handleUpdate() {
    while (true) {
        socket->read(buffer.data(), 26);

        auto message = Snapcast::Message();
        message.readBaseMessage(buffer.data());

        BELL_LOG(info, "snapcast", "Received message of type %d and length %d", message.type, message.msgLength);

        auto totalRead = 0;
        while (totalRead < message.msgLength) {
            auto read = socket->read(buffer.data(), message.msgLength - totalRead);
            totalRead += read;
        }

        switch (message.type) {
        case Snapcast::Message::MessageType::CodecHeader: {
            BELL_LOG(info, "snapcast", "Received codec header");
            auto codecHeader = Snapcast::CodecHeaderMessage();
            codecHeader.deserialize(buffer);

            std::cout << codecHeader.codecString << std::endl;

            if (codecHeader.codecString == "opus") {
                BELL_LOG(info, "snapcast",
                         "Received opus codec header sample rate %d bits %d channels %d",
                         codecHeader.sampleRate, codecHeader.bits,
                         codecHeader.channels);
                decoder->setup(codecHeader.sampleRate, codecHeader.channels,
                               codecHeader.bits);
                outputConfigCallback(codecHeader.sampleRate, codecHeader.bits);
            } else {
                BELL_LOG(error, "snapcast", "Unknown codec %s",
                         codecHeader.codecString.c_str());
            }

            break;
        }
        case Snapcast::Message::MessageType::WireChunk:
            auto readPos = 0;
            int32_t sec;
            int32_t usec;
            uint32_t dataSize;

            readPos += message.readVal(buffer.data(), sec);
            readPos += message.readVal(buffer.data() + readPos, usec);
            readPos += message.readVal(buffer.data() + readPos, dataSize);

            uint32_t readDataSize = 0;
            auto res = decoder->decode(buffer.data() + readPos, dataSize, readDataSize);
            dataCallback(res, readDataSize);
            break;
        }
    }
}