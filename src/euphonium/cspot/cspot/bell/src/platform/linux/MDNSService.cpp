#include "platform/MDNSService.h"

/**
 * Linux implementation of MDNSService using avahi.
 * @see https://www.avahi.org/doxygen/html/
 **/
void MDNSService::registerService(
    const std::string& serviceName,
    const std::string& serviceType,
    const std::string& serviceProto,
    const std::string& serviceHost,
    int servicePort,
    const std::map<std::string, std::string> txtData
) {
   //TODO: add avahi stuff
}
