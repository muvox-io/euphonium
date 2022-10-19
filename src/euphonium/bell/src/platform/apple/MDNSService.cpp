#include "platform/MDNSService.h"
#include "dns_sd.h"
#include <arpa/inet.h>

/**
 * MacOS implementation of MDNSService.
 * @see https://developer.apple.com/documentation/dnssd/1804733-dnsserviceregister
 **/
void MDNSService::registerService(
    const std::string& serviceName,
    const std::string& serviceType,
    const std::string& serviceProto,
    const std::string& serviceHost,
    int servicePort,
    const std::map<std::string, std::string> txtData
) {
    DNSServiceRef ref = NULL;
    TXTRecordRef txtRecord;
    TXTRecordCreate(&txtRecord, 0, NULL);
    for (auto& data : txtData) {
        TXTRecordSetValue(&txtRecord, data.first.c_str(), data.second.size(), data.second.c_str());
    }
    DNSServiceRegister(
        &ref, /* sdRef */
        0, /* flags */
        0, /* interfaceIndex */
        serviceName.c_str(), /* name */
        (serviceType + "." + serviceProto).c_str(), /* regType (_spotify-connect._tcp) */
        NULL, /* domain */
        NULL, /* host */
        htons(servicePort), /* port */
        TXTRecordGetLength(&txtRecord), /* txtLen */
        TXTRecordGetBytesPtr(&txtRecord), /* txtRecord */
        NULL, /* callBack */
        NULL /* context */
    );
    TXTRecordDeallocate(&txtRecord);
}
