#pragma once

#include <atomic>
#include <memory>
#include <mutex>
// for std::hash
#include <functional>
#include <string>

#include "BellMQTTClient.h"
#include "BellTask.h"
#include "HTTPClient.h"
#include "Queue.h"

#include "SystemPlugin.h"
#include "EuphContext.h"
#include "EuphLogger.h"
#include "WrappedSemaphore.h"

namespace euph {

class MQTTVmEvent : public Event {
 public:
  std::string topic, message;

  MQTTVmEvent(std::string topic, std::string message): topic(topic), message(message) {
    this->eventType = EventType::VM_MAIN_EVENT;
    this->subType = "mqtt_publish_received";
  }

  berry::map toBerry() override {
    return {
      {"topic", this->topic},
      {"message", this->message}
    };
  }
};

class MQTTPlugin : public SystemPlugin, public bell::Task {
 public:
  MQTTPlugin(std::shared_ptr<euph::Context> ctx);
  ~MQTTPlugin();

  struct MessageRequest {
    std::string topic;
    std::string payload;

    enum class Type {
      PUBLISH,
      SUBSCRIBE
    } type;
  };

  void runPlugin();

  // --- SystemPlugin implementation
  void initializeBindings() override;
  std::string getName() override;

  // -- BellTask implementation
  void runTask() override;

  // -- Berry bindings
  void _configure(std::string host, int port, std::string username, std::string password);
  void _publish(std::string topic, std::string payload);
  void _subscribe(std::string topic);
  void _disconnect();

 private:
  std::atomic<bool> isConnected = false;
  std::string host, username, password;
  int port;

  bell::Queue<MessageRequest> messageQueue;
  std::shared_ptr<euph::Context> ctx;
  std::unique_ptr<bell::WrappedSemaphore> connectionRequested;
  std::unique_ptr<bell::MQTTClient> client;

  std::atomic<bool> isRunning = false;
  std::mutex runningMutex, connectionMutex;
};
}  // namespace euph
