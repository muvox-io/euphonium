#ifndef CONFIGURATION_API_SERVICE_H
#define CONFIGURATION_API_SERVICE_H

#include <optional>
#include <memory>
#include "Core.h"
#include "Service.h"
#include "protobuf.h"

#include <sstream>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string>
#include <netdb.h>
#ifndef SOCK_NONBLOCK
#include <fcntl.h>
#define SOCK_NONBLOCK O_NONBLOCK
#endif

class ConfigurationApiService : public Service {
private:
    ConfigurationApiServiceConfiguration config;
    std::shared_ptr<Core> core;
    int serverPort = 2138;
public:
    ConfigurationApiService(std::shared_ptr<Core> core) {
        serviceName = "configurationApiService";
        this->core = core;
    };

    AnyRef getConfigurationRef() {
        return AnyRef::of(&config);
    }

    void runConfigurationServer();
    void init() {
        runConfigurationServer();
    }
};

#endif