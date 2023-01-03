#pragma once

#include <string>
#include <mutex>
#include <string_view>

#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "nvs_handle.hpp"

#include "Connectivity.h"
#include "EuphLogger.h"

namespace euph {
class ESP32Connectivity : public Connectivity {
 public:
  ESP32Connectivity(std::shared_ptr<euph::EventBus> eventBus);
  ~ESP32Connectivity(){};

  void initConfiguration();
  void initializeWiFiStack();

  void handleEvent(esp_event_base_t event_base, int32_t event_id,
                   void* event_data);
  void registerHandlers(std::shared_ptr<bell::BellHTTPServer> http) override;

  void initializeAP();

 private:
  std::string TAG = "ESP32Connectivity";
  std::string nvsWiFiKey = "wifi_settings";
  std::string ssid;
  std::string password;
};
}  // namespace euph
