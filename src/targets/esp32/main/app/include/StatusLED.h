#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include <driver/gpio.h>
#include <driver/ledc.h>

#include "WrappedSemaphore.h"

#include "Connectivity.h"
#include "EventBus.h"

#define STATUS_R 27
#define STATUS_G 2
#define STATUS_B 4

namespace euph {
class StatusLED: public bell::Task, public euph::EventSubscriber  {
public:
  StatusLED(std::shared_ptr<euph::EventBus> eventBus);

  enum class Behaviour {
    BREATHING,
    BREATHING_FAST,
    FLASHING,
    FLASHING_FAST,
    ON
  };

  struct ModeDefinition {
    int r, g, b;
    Behaviour behaviour;
  };

  enum class StatusEvent {
    WIFI_NO_CONFIG,
    WIFI_CONNECTING,
    WIFI_ERROR_HAS_AP,

    NORMAL_IDLE,
    NORMAL_PLAYBACK,

    BLUETOOTH_IDLE,
    BLUETOOTH_PLAYBACK,

    ERROR
  };

  std::unordered_map<StatusEvent, ModeDefinition> definitions = {
    { StatusEvent::WIFI_NO_CONFIG,    { .r = 70, .g = 70, .b = 0, .behaviour = Behaviour::BREATHING } },
    { StatusEvent::WIFI_CONNECTING,   { .r = 70, .g = 70, .b = 0, .behaviour = Behaviour::BREATHING_FAST } },
    { StatusEvent::WIFI_ERROR_HAS_AP, { .r = 70, .g = 70, .b = 0, .behaviour = Behaviour::ON } },

    { StatusEvent::NORMAL_IDLE,       { .r = 0, .g = 70, .b = 0, .behaviour = Behaviour::ON } },
    { StatusEvent::NORMAL_PLAYBACK,   { .r = 0, .g = 70, .b = 0, .behaviour = Behaviour::BREATHING } },

    { StatusEvent::BLUETOOTH_IDLE,   { .r = 0, .g = 0, .b = 70, .behaviour = Behaviour::BREATHING } },
    { StatusEvent::BLUETOOTH_PLAYBACK,   { .r = 0, .g = 0, .b = 70, .behaviour = Behaviour::ON } },
  };

  ModeDefinition currentStatus = {
    .r = 70,
    .g = 30,
    .b = 0,
    .behaviour = Behaviour::BREATHING
  };

  void runTask() override;
  void handleEvent(std::unique_ptr<Event>& event) override;
private:
  std::shared_ptr<euph::EventBus> eventBus;
  std::unique_ptr<bell::WrappedSemaphore> statusUpdated;

  void setupTimer();
  void updateLEDChannel(ledc_channel_t channel, int gpioNum);
};
}
