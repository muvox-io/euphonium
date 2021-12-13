#include "SSDPListener.h"
#include "HTTPClient.h"
#include "cJSON.h"

SSDPListener::SSDPListener(std::string &uuid) : bell::Task("ssdp", 1024, 1) {
    this->uuid = uuid;
    startTask();
}

const std::string currentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tstruct);

    return buf;
}

void SSDPListener::listenForMulticast() {
    // create what looks like an ordinary UDP socket
    //
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket");
        return;
    }

    // allow multiple sockets to use the same PORT number
    //
    u_int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(yes)) <
        0) {
        perror("Reusing ADDR failed");
        return;
    }

    // set up destination address
    //
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // differs from sender
    addr.sin_port = htons(1900);

    // bind to receive address
    //
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return;
    }

    // use setsockopt() to request that the kernel join a multicast group
    //
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr("239.255.255.250");
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq,
                   sizeof(mreq)) < 0) {
        perror("setsockopt");
        return;
    }

    int nbytes;
    auto readBuffer = std::vector<uint8_t>(128);
    auto currentLine = std::string();

    // now just enter a read-print loop
    while (isRunning) {
        socklen_t addrlen = sizeof(addr);
        int nbytes = recvfrom(fd, readBuffer.data(), 128, 0,
                              (struct sockaddr *)&addr, &addrlen);
        if (nbytes < 0) {
            perror("recvfrom");
            return;
        }
        currentLine +=
            std::string(readBuffer.data(), readBuffer.data() + nbytes);
        while (currentLine.find("\r\n") != std::string::npos) {
            auto line = currentLine.substr(0, currentLine.find("\r\n"));
            currentLine = currentLine.substr(currentLine.find("\r\n") + 2,
                                             currentLine.size());
            if (line.find("ST: ") != std::string::npos) {
                BELL_SLEEP_MS(700);

                std::stringstream stream;
                stream << "HTTP/1.1 200 OK\r\n";
                stream << "LOCATION: http://192.168.1.208:80/ssdp/device-desc.xml\r\n";
                stream << "CACHE-CONTROL: max-age=1800\r\n";
                stream << "EXT: \r\n";
                stream << "SERVER: UPnP/1.0\r\n";
                stream << "BOOTID.UPNP.ORG: 1\r\n";
                stream << "USN: uuid:" << uuid << "\r\n";
                stream << "ST: urn:dial-multiscreen-org:service:dial:1\r\n";

                auto data = stream.str();
                int res = sendto(fd, data.data(), data.size(), 0,
                                 (struct sockaddr *)&addr, addrlen);
            }
        }
    }
}

void SSDPListener::runTask() {

    listenForMulticast();
}
