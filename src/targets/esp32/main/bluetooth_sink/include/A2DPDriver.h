#pragma once

#include <memory>
#include <string>
#include <variant>

#include <EuphLogger.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "BellTask.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"
#include "esp_bt.h"
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_log.h"

extern "C" {
void local_set_volume(uint8_t volume);
void local_active_audio(bool active);
void local_metadata_updated(char* artist, char* album, char* title,
                            int duration);
void local_set_sr(uint32_t samplerate);
#include "bt_app_av.h"
#include "bt_app_core.h"
}

namespace euph {
class A2DPDriver {
 public:
  A2DPDriver();
  ~A2DPDriver();

  enum class EventType { VOLUME, PLAYBACK_STATE, PLAYBACK_METADATA, PLAYBACK_SAMPLERATE };

  struct PlaybackMetadata {
    std::string artist;
    std::string album;
    std::string title;
    int duration;
  };

  typedef std::variant<uint8_t, PlaybackMetadata, bool, uint32_t> EventData;
  struct Event {
    EventType type;
    EventData data;
  };

  typedef std::function<void(const uint8_t* data, size_t len)> PcmCallback;
  typedef std::function<void(Event&)> EventCallback;

  PcmCallback dataHandler;
  EventCallback eventHandler;

  void initialize(const std::string& name);
  void disconnect();
  void processQueue();

 private:
  std::string TAG = "A2DPDriver";
};
}  // namespace euph
