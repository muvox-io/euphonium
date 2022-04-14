#ifndef EUPHONIUM_MQTT_PLUGIN_H
#define EUPHONIUM_MQTT_PLUGIN_H

#include "ScriptLoader.h"
#include "EuphoniumLog.h"
#include "Logger.h"
#include "CoreEvents.h"
#include "Module.h"
#include "Core.h"
#include <atomic>
#include <mutex>
#include "Queue.h"
#include "Task.h"
#include "mqtt.h"
#include <EventBus.h>
#include <memory.h>
#include "Task.h"

class MQTTPublishReceivedEvent: public Event {
    public:
    std::string topic;
    std::string message;

    MQTTPublishReceivedEvent(std::string& topic, std::string& message) {
        this->topic = topic;
        this->message = message;
        this->subType = "mqttPublishEvent";
        this->eventType = EventType::LUA_MAIN_EVENT;
    };

    berry::map toBerry() {
        berry::map result;
        result["topic"]  = topic;
        result["message"] = message;
        return result;
    }
};

enum class MQTTMessageType {
    PUBLISH,
    SUBSCRIBE
};

struct MQTTMessage {
    MQTTMessageType type;
    std::string topic;
    std::string message;
};

class MQTTPlugin : public bell::Task, public Module
{
private:
    bool clientInitialized = false;
    struct mqtt_client client;
    uint8_t sendbuf[2048];
    uint8_t recvbuf[1024];
    bell::Queue<MQTTMessage> mqttQueue;
    std::mutex connectedMutex;

public:
    MQTTPlugin();
    void loadScript(std::shared_ptr<ScriptLoader> scriptLoader);
    void setupBindings();
    void shutdown();
    void configurationUpdated();
    void startAudioThread();
    bool connectToBroker(std::string url, std::string port, std::string user, std::string password);
    void publish(std::string topic, std::string msg);
    void subscribe(std::string topic);
    void runTask();
};

#endif
