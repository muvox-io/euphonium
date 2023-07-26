#include "MQTTPlugin.h"
#include <mutex>
#include "BellMQTTClient.h"
#include "BellTask.h"
#include "EuphLogger.h"

using namespace euph;

MQTTPlugin::MQTTPlugin(std::shared_ptr<euph::Context> ctx)
    : bell::Task("MQTTPlugin", 1024, 0, 0) {
  this->ctx = ctx;
  this->connectionRequested = std::make_unique<bell::WrappedSemaphore>(5);
  this->client = std::make_unique<bell::MQTTClient>();

  this->client->setPublishCallback(
      [ctx](std::string topic, std::string payload) {
        // Create a new event
        auto event = std::make_unique<MQTTVmEvent>(topic, payload);

        // Relay the message to the VM
        ctx->eventBus->postEvent(std::move(event));
      });
}

MQTTPlugin::~MQTTPlugin() {}

void MQTTPlugin::initializeBindings() {
  // Export native code to berry
  this->ctx->vm->export_this("_connect", this, &MQTTPlugin::_configure, "mqtt");
  this->ctx->vm->export_this("_publish", this, &MQTTPlugin::_publish, "mqtt");
  this->ctx->vm->export_this("_disconnect", this, &MQTTPlugin::_disconnect,
                             "mqtt");
  this->ctx->vm->export_this("_subscribe", this, &MQTTPlugin::_subscribe,
                             "mqtt");
}

std::string MQTTPlugin::getName() {
  return "mqtt";
}

void MQTTPlugin::runTask() {
  isRunning = true;

  std::scoped_lock lock(this->runningMutex);
  while (isRunning) {
    // Wait for connection request
    connectionRequested->wait();

    // Connect to the broker
    EUPH_LOG(info, TASK, "Connecting to MQTT broker at %s:%d",
             this->host.c_str(), this->port);

    std::scoped_lock connectionLock(this->connectionMutex);
    try {
      this->client->connect(this->host, this->port, this->username,
                            this->password);
      // Mark as connected
      isConnected = true;

      EUPH_LOG(info, TASK, "Broker connected, waiting for messages");

      // Report notification to the UI
      this->ctx->eventBus->postEvent(std::make_unique<NotificationEvent>(
          NotificationEvent::Type::INFO, "mqtt", "MQTT broker connected",
          this->host));

      // Holds incoming message request
      MessageRequest request;

      while (isConnected) {
        // Handler client's request if available, sync with the broker otherwise
        if (this->messageQueue.wtpop(request, 100)) {
          switch (request.type) {
            case MessageRequest::Type::PUBLISH:
              this->client->publish(request.topic, request.payload);
              break;
            case MessageRequest::Type::SUBSCRIBE:
              this->client->subscribe(request.topic);
              break;
          }
        } else {
          this->client->sync();
        }
      }

    } catch (...) {
      EUPH_LOG(error, TASK, "Failed to connect to MQTT broker");
      // TODO relay to VM
    }

    if (isConnected) {
      // Disconnect the client
      this->client->disconnect();

      // Mark as disconnected
      isConnected = false;
    }
  }
}

void MQTTPlugin::_configure(std::string host, int port, std::string username,
                            std::string password) {
  // Ensure the task is running
  if (!isRunning) {
    this->startTask();
  }

  if (this->isConnected) {
    // Disconnect the client
    isConnected = false;

    // Wait for the connection to be closed
    std::scoped_lock connectionLock(this->connectionMutex);
  }

  // Assign the configuration
  this->host = host;
  this->port = port;
  this->username = username;
  this->password = password;

  // Request a connection
  this->connectionRequested->give();
}

void MQTTPlugin::_publish(std::string topic, std::string payload) {
  // Push the request to the queue
  this->messageQueue.push({topic, payload, MessageRequest::Type::PUBLISH});
}

void MQTTPlugin::_subscribe(std::string topic) {
  // Push the request to the queue
  this->messageQueue.push({topic, "", MessageRequest::Type::SUBSCRIBE});
}

void MQTTPlugin::_disconnect() {
  if (!isConnected)
    return;

  // Disconnect the client
  isConnected = false;

  // Wait for the connection to be closed
  std::scoped_lock lock(this->connectionMutex);
}