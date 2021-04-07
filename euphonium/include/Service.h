#ifndef EUPHONIUM_SERVICE_H
#define EUPHONIUM_SERVICE_H

#include <string>
#include <memory>
#include <optional>
#include "protobuf.h"
#include "SystemInfo.h"
#include "EventBus.h"


class Service {
    public:
    Service() {};
    virtual ~Service() {}

    std::string serviceName;

    virtual AnyRef getConfigurationRef() = 0;
    virtual void init() = 0;
};

#endif