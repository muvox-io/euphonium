#ifndef EUPHONIUM_CONFIG_PERSISTOR_MODULE_H
#define EUPHONIUM_CONFIG_PERSISTOR_MODULE_H

#include "ScriptLoader.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <memory>
#include "Queue.h"
#include "Module.h"
#include "Task.h"
#include "EventBus.h"
#include "EuphoniumLog.h"

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

struct PersistenceRequest {
    bool isSave = false;
    std::string key;
    std::string value;
};


class ConfigPersistor : public bell::Task, public Module
{
private:
    bell::Queue<PersistenceRequest> requestQueue;

public:
    ConfigPersistor();
    std::shared_ptr<ScriptLoader> scriptLoader;
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader);
    void setupBindings();
    void persist(std::string key, std::string value);
    void load(std::string key);
    void runTask();
    void startAudioThread();
    void shutdown() {};
};

extern std::shared_ptr<ConfigPersistor> mainPersistor;

#endif
