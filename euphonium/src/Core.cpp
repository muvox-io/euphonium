#include "Core.h"
#include "DupaService.h"
#include "ConfigurationApiService.h"
#include "SystemInfo.h"
#include <string.h>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

Core::Core() {
}

void Core::registerChildren() {
    this->registeredServices.push_back(std::make_shared<DupaService>(shared_from_this()));
    this->registeredServices.push_back(std::make_shared<ConfigurationApiService>(shared_from_this()));
}

void Core::logAvailableServices() {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    
    int r = luaL_loadstring(L, "print(\"Hello World\")");
    r = lua_pcall(L, 0, 0, 0);
    lua_close(L);
}

std::shared_ptr<Service> Core::getServiceWithName(std::string& name) {
    // for (const auto& service : this->registeredServices) {
    //     if (service->serviceName == name) {
    //         return service;
    //     }
    // }

    // return nullptr;
}