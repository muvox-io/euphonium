#include "Core.h"
#include "DupaService.h"
#include "ConfigurationApiService.h"
#include "HTTPServer.h"
#include "SystemInfo.h"
#include <string.h>
#include <sol.hpp>
#include <cassert>

Core::Core() {
}

void Core::registerChildren() {
    this->registeredServices.push_back(std::make_shared<DupaService>(shared_from_this()));
    this->registeredServices.push_back(std::make_shared<ConfigurationApiService>(shared_from_this()));
}

void Core::logAvailableServices() {
    sol::state lua;
    int x = 0;
    lua.set_function("beep", [&x]{ ++x; });
    lua.script("beep()");
    assert(x == 0);
}

std::shared_ptr<Service> Core::getServiceWithName(std::string& name) {
    // for (const auto& service : this->registeredServices) {
    //     if (service->serviceName == name) {
    //         return service;
    //     }
    // }

    // return nullptr;
}