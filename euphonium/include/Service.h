#ifndef EUPHONIUM_SERVICE_H
#define EUPHONIUM_SERVICE_H

#include <string>
#include <memory>
#include "protobuf.h"
#include "Core.h"

class Service {
    public:
    Service() {};
    virtual ~Service() {}

    std::string serviceName;
    std::shared_ptr<Core> core;

    virtual AnyRef getConfigurationRef() = 0;
    virtual void init() = 0;
};

#endif