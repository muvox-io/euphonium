#ifndef EUPHONIUM_CONFIG_PERSISTOR_MODULE_H
#define EUPHONIUM_CONFIG_PERSISTOR_MODULE_H

#include "BaseHTTPServer.h"
#include "ScriptLoader.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <memory>
#include "Queue.h"
#include "Module.h"
#include "Task.h"
#include "EventBus.h"
#include "HTTPServer.h"
#include "EuphoniumLog.h"
#include "plugins/http/HTTPInstance.h"

class ConfigLoadedEvent: public Event {
    public:
    std::string key, value;
    ConfigLoadedEvent(std::string& key, std::string& value) {
        this->key = key;
        this->value = value;
        this->subType = "handleConfigLoaded";
        this->eventType = EventType::LUA_MAIN_EVENT;
    };

    berry::map toBerry() {
        berry::map result;
        result["key"] = key;
        result["value"] = value;
        return result;
    }
};

enum class StorageOperationType {
    PERSIST_CONF,
    READ_CONF,
    WRITE_HTTP
};

struct StorageOperation {
    StorageOperationType type;
    std::string key;
    std::string value;
    int connFd;
};


class ConfigPersistor : public bell::Task, public Module
{
private:
    bell::Queue<StorageOperation> requestQueue;

public:
    ConfigPersistor();
    std::shared_ptr<ScriptLoader> scriptLoader;
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader);
    void setupBindings();
    void persist(std::string key, std::string value);
    void load(std::string key);
    void runTask();
    void serveFile(int fd, std::string& fileName);
    void startAudioThread();
    void shutdown() {};
};

extern std::shared_ptr<ConfigPersistor> mainPersistor;

#endif
