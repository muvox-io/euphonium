#ifndef DUPA_H
#define DUPA_H

#include <optional>
#include <memory>
#include "Core.h"
#include "Service.h"
#include "protobuf.h"

class DupaService : public Service {
private:
    DupaServiceConfiguration config;
public:
    DupaService(std::shared_ptr<Core> core) {
        serviceName = "dupaService";
    };

    AnyRef getConfigurationRef() {
        return AnyRef::of(&config);
    }

    void init() {};
};

#endif