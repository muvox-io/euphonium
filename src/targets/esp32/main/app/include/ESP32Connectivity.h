#pragma once

#include <mutex>
#include <string>
#include <string_view>

#include <lwip/ip4_addr.h>
#include "esp_event.h"
#include "esp_wifi.h"
#include "fmt/format.h"
#include "nlohmann/json.hpp"
#include "nvs.h"
#include "nvs_flash.h"
#include "nvs_handle.hpp"

#include "BellTask.h"
#include "CaptivePortalTask.h"
#include "WrappedSemaphore.h"

#include "Connectivity.h"
#include "EuphLogger.h"

namespace euph {
class ESP32Connectivity : public Connectivity, public bell::Task {
 public:
  ESP32Connectivity(std::shared_ptr<euph::EventBus> eventBus);
  ~ESP32Connectivity(){};

  void initConfiguration();
  void initializeWiFiStack();

  void persistConfig();
  void requestScan();
  void attemptConnect(const std::string& ssid, const std::string& passwd);
  void handleEvent(esp_event_base_t event_base, int32_t event_id,
                   void* event_data);
  void registerHandlers(std::shared_ptr<bell::BellHTTPServer> http) override;
  void initializeAP();
  void initializeSTA();
  void displayNameLoaded(std::string& name) override;

  /**
   * @brief Request an asynchronous clearing of the wifi configuration.
   *
   * Can be called from PSRAM tasks. 
   */
  void requestClearConfig() override;

  void runTask() override;

 private:
  std::string TAG = "ESP32Connectivity";

  uint8_t DEFAULT_SCAN_LIST_SIZE = 10;
  uint8_t MAX_CONNECTION_ATTEMPTS = 3;
  wifi_ap_record_t scanInfo[10];

  std::string apName = "Euphonium";
  std::string nvsWiFiKey = "wifi_settings";
  std::string displayName;
  std::string ssid;
  std::string password;
  nlohmann::json jsonBody = {};

  int connectionAttempts = 0;
  std::atomic<bool> isScanning = false;
  bool isApMode = false;

  esp_netif_t* staNetif = nullptr;
  esp_netif_t* apNetif = nullptr;

  std::unique_ptr<bell::WrappedSemaphore> dataUpdateSemaphore;
  std::unique_ptr<euph::CaptivePortalTask> captivePortalDNS;

  std::atomic<bool> clearConfigRequested = false;
  /**
   * @brief Clears wifi configuration from NVS.
   * 
   * Shall only be called from a non-PSRAM task.
   */
  void clearConfig();
};
}  // namespace euph
