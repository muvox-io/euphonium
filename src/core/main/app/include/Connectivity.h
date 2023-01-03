#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include "BellHTTPServer.h"
#include "EventBus.h"
#include "HTTPDispatcher.h"
#include "nlohmann/json_fwd.hpp"

namespace euph {
class Connectivity {
protected:
  std::shared_ptr<EventBus> eventBus;

public:
  Connectivity() = default;
  ~Connectivity() = default;

  enum class State {
    DISCONNECTED,
    CONNECTING,
    CONNECTED_NO_INTERNET,
    CONNECTED,
  };

  enum class ConnectivityType { WIFI_STA, WIFI_AP, DEFAULT };

  static std::string stateToString(State state) {
    switch (state) {
      case State::DISCONNECTED:
        return "DISCONNECTED";
      case State::CONNECTING:
        return "CONNECTING";
      case State::CONNECTED_NO_INTERNET:
        return "CONNECTED_NO_INTERNET";
      case State::CONNECTED:
        return "CONNECTED";
      default:
        return "UNKNOWN";
    }
  }

  static std::string typeToString(ConnectivityType type) {
    switch (type) {
      case ConnectivityType::WIFI_STA:
        return "WIFI_STA";
      case ConnectivityType::WIFI_AP:
        return "WIFI_AP";
      case ConnectivityType::DEFAULT:
        return "DEFAULT";
      default:
        return "UNKNOWN";
    }
  }


  struct ConnectivityData {
    State state;
    ConnectivityType type;

    std::string ssid;
    std::string ipAddr;

    std::string jsonBody = "{}";


    nlohmann::json toJson() {
      auto subJsonBody = nlohmann::json::parse(jsonBody);

      nlohmann::json connectivityEventJson = {
                  {"state", Connectivity::stateToString(state)},
                  {"type", Connectivity::typeToString(type)},
                  {"ssid", ssid},
                  {"ip", ipAddr},
                  {"body", subJsonBody},
      };

      return connectivityEventJson;
    }
  };

  class ConnectivityEvent : public Event {
   public:
    ConnectivityEvent() { eventType = EventType::CONNECTIVITY_EVENT; }
    ~ConnectivityEvent() = default;

    ConnectivityData data;

    berry::map toBerry() override {
      berry::map eventMap;
      return eventMap;
    }
  };

  ConnectivityData data;

  ConnectivityData& getData() {
    return this->data;
  }

  void setEventBus(std::shared_ptr<EventBus> eventBus) {
    this->eventBus = eventBus;
  }

  void sendStateUpdate() {
    auto event = std::make_unique<ConnectivityEvent>();
    event->data = this->data;
    eventBus->postEvent(std::move(event));
  }

  virtual void registerHandlers(std::shared_ptr<bell::BellHTTPServer> http) = 0;
};
}  // namespace euph
