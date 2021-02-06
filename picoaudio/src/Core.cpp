#include "Core.h"
#include "DupaService.h"
#include "ConfigurationApiService.h"

Core::Core() {
}

void Core::registerChildren() {
    this->registeredServices.push_back(std::make_shared<DupaService>(shared_from_this()));
    this->registeredServices.push_back(std::make_shared<ConfigurationApiService>(shared_from_this()));
}

void Core::logAvailableServices() {
    for (const auto& service : this->registeredServices) {
        // std::cout << service->serviceName << std::endl;
        service->init();
    }
}

std::shared_ptr<Service> Core::getServiceWithName(std::string& name) {
    for (const auto& service : this->registeredServices) {
        if (service->serviceName == name) {
            return service;
        }
    }

    return nullptr;
}