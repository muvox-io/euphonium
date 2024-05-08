#pragma once

#include <memory>
#include <semaphore>
#include <unordered_map>
#include <utility>

#include <driver/gpio.h>
#include <driver/ledc.h>

#include <BellTask.h>

#include "Connectivity.h"
#include "EventBus.h"

#define STATUS_R 27
#define STATUS_G 2
#define STATUS_B 4

namespace euph {
class StatusLED : public bell::Task, public euph::EventSubscriber {
 public:
  StatusLED(std::shared_ptr<euph::EventBus> eventBus);

  enum class Behaviour {
    BREATHING,
    BREATHING_FAST,
    FLASHING,
    FLASHING_FAST,
    PERIODIC_DIMMING,
    ON
  };

  struct ModeDefinition {
    /**
     * @brief Modes with higher priority will override lower priority modes.
     * 
     */
    int prio;
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

    EMERGENCY_MODE,

    ERROR
  };

  std::unordered_map<StatusEvent, ModeDefinition> definitions = {
      // clang-format off
    { StatusEvent::WIFI_NO_CONFIG,     { .prio = 10, .r = 70, .g = 70, .b = 0, .behaviour = Behaviour::BREATHING } },
    { StatusEvent::WIFI_CONNECTING,    { .prio = 10, .r = 70, .g = 70, .b = 0, .behaviour = Behaviour::BREATHING_FAST } },
    { StatusEvent::WIFI_ERROR_HAS_AP,  { .prio = 10, .r = 70, .g = 70, .b = 0, .behaviour = Behaviour::ON } },

    { StatusEvent::NORMAL_IDLE,        { .prio = 10, .r = 0, .g = 70, .b = 0, .behaviour = Behaviour::ON } },
    { StatusEvent::NORMAL_PLAYBACK,    { .prio = 10, .r = 0, .g = 70, .b = 0, .behaviour = Behaviour::BREATHING } },

    { StatusEvent::BLUETOOTH_IDLE,     { .prio = 10, .r = 0, .g = 0, .b = 70, .behaviour = Behaviour::BREATHING } },
    { StatusEvent::BLUETOOTH_PLAYBACK, { .prio = 10, .r = 0, .g = 0, .b = 70, .behaviour = Behaviour::ON } },

    { StatusEvent::EMERGENCY_MODE,     { .prio = 50, .r = 255, .g = 0, .b = 0, .behaviour = Behaviour::PERIODIC_DIMMING } },
      // clang-format on

  };

  ModeDefinition currentStatus = {.prio = 0,
                                  .r = 70,
                                  .g = 30,
                                  .b = 0,
                                  .behaviour = Behaviour::BREATHING};

  void runTask() override;
  void handleEvent(std::unique_ptr<Event>& event) override;

 private:
  std::shared_ptr<euph::EventBus> eventBus;
  std::counting_semaphore<10> statusUpdated;

  void setupTimer();
  void updateLEDChannel(ledc_channel_t channel, int gpioNum);

  /**
   * @brief Switches the current status to the new status, only if the new status
   * has a higher or equal priority.
   * 
   * @param newStatus The new status to switch to.
   */
  void switchCurrentStatus(ModeDefinition newStatus);
};
}  // namespace euph
