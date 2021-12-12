#ifndef EUPH_DIAL_SERVER
#define EUPH_DIAL_SERVER

#include <HTTPServer.h>
#include <memory>
#include "../http/HTTPModule.h"
#include "BellUtils.h"


class DIALServer {
    private:
        std::string deviceId;

  public:
    DIALServer(std::string& deviceId) {
        this->deviceId = deviceId;
    }
    ~DIALServer() {}

    void registerHandlers(std::shared_ptr<bell::HTTPServer> server);
};

#endif
