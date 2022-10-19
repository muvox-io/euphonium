#ifndef EUPH_DIAL_SERVER
#define EUPH_DIAL_SERVER

#include "../http/HTTPModule.h"
#include "BellUtils.h"
#include "YouTubeLeanHandler.h"
#include <HTTPServer.h>
#include <memory>

class DIALServer {
  private:
    std::string deviceId;
    std::shared_ptr<YouTubeLeanHandler> leanHandler;

  public:
    DIALServer(std::string &deviceId,
               std::shared_ptr<YouTubeLeanHandler> leanHandler) {
        this->deviceId = deviceId;
        this->leanHandler = leanHandler;
    }
    ~DIALServer() {}

    void registerHandlers(std::shared_ptr<bell::HTTPServer> server);
};

#endif
