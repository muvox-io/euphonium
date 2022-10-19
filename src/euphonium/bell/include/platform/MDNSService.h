#ifndef BELLL_MDNS_SERVICE_H
#define BELLL_MDNS_SERVICE_H
#include <string>
#include <map>

class MDNSService {
public:
    static void registerService(
        const std::string &serviceName,
        const std::string &serviceType,
        const std::string &serviceProto,
        const std::string &serviceHost,
        int servicePort,
        const std::map<std::string, std::string> txtData
    );
};

#endif
