#include "SnapcastConnection.h"
#include "BellUtils.h"
SnapcastConnection::SnapcastConnection() {
    this->socket = std::make_unique<bell::TCPSocket>();
}

void SnapcastConnection::writeHello() {
    bell::JSONObject json;
    json["Arch"] = "x86_64";
    json["ClientName"] = "Euphonium";
    json["HostName"] = "euphonium";
    json["ID"] = "00:11:22:33:44:55";
    json["Instance"] = 1;
    json["MAC"] = "00:11:22:33:44:55";
    json["OS"] = "Arch Linux";
    json["SnapStreamProtocolVersion"] = 2;
    json["Version"] = "0.26.0";
    auto message = Snapcast::HelloMessage();
    message.setData("x86_64", "Euphonium", "euphonium", "00:11:22:33:44:55", 1, "00:11:22:33:44:55", "Arch Linux", 2, "0.0.1");
    auto serialized = message.serialize();

    std::cout << serialized.size() << std::endl;
    auto size = this->socket->write(serialized.data(), serialized.size());
    std::cout << size << std::endl;
}

void SnapcastConnection::connectWithServer(const std::string url) {
    socket->open("192.168.1.18", 1704);
    writeHello();
    auto buffer = std::vector<uint8_t>(1024);
    while(true) {

        auto read = socket->read(buffer.data(), 1024);
        if (read > 0) {
            printf("%lu\n", read);
            printf("%d\n", buffer[0]);
        } else {
            BELL_SLEEP_MS(500);
        }
    }
}