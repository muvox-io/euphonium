#ifndef PICOAUDIO_CORE_H
#define PICOAUDIO_CORE_H

#include <vector>
#include <memory>
#include <iostream>

class Service;

class Core : public std::enable_shared_from_this<Core> {
private:
    std::vector<std::shared_ptr<Service>> registeredServices;

public:
    Core();
    void registerChildren();
    void logAvailableServices();

    std::shared_ptr<Service> getServiceWithName(std::string& name);
};

#endif