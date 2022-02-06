#ifndef EUPHONIUM_HTTP_DEVTOOLS
#define EUPHONIUM_HTTP_DEVTOOLS

#include "HTTPServer.h"
#include "HTTPInstance.h"
#include <memory.h>
#include "ScriptLoader.h"
#include <iostream>
#include <fstream>
#include <thread>
#include "Module.h"
#include "Task.h"
#include "EuphoniumLog.h"
#include "dirent.h"
#include "plugins/persistor/ConfigPersistor.h"

class HTTPDevTools {
    public:
    HTTPDevTools() {};

    static void registerHandlers(std::shared_ptr<bell::HTTPServer> server);
};

#endif