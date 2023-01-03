#include "ESP32Connectivity.h"

using namespace euph;

static void wifiEventHandler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data) {
  ESP32Connectivity* self = static_cast<ESP32Connectivity*>(arg);

  // Relay event to the class instance
  self->handleEvent(event_base, event_id, event_data);
}

ESP32Connectivity::ESP32Connectivity(std::shared_ptr<euph::EventBus> eventBus) {
  this->eventBus = eventBus;

  this->data = {
      Connectivity::State::DISCONNECTED,
      Connectivity::ConnectivityType::DEFAULT,
  };

  this->initializeWiFiStack();
  this->initConfiguration();
};

void ESP32Connectivity::initConfiguration() {
  esp_err_t err;

  std::unique_ptr<nvs::NVSHandle> handle =
      nvs::open_nvs_handle("storage", NVS_READWRITE, &err);
  if (err != ESP_OK) {
    EUPH_LOG(error, TAG, "Error (%s) opening NVS handle!\n",
             esp_err_to_name(err));
  } else {
    EUPH_LOG(info, TAG, "Reading WiFi configuration from NVS storage...");

    size_t wifiConfigSize = 0;
    handle->get_item_size(nvs::ItemType::SZ, nvsWiFiKey.c_str(),
                          wifiConfigSize);

    if (wifiConfigSize > 0) {
      EUPH_LOG(info, TAG, "Found existing WiFi configuration, connecting...");
    } else {
      EUPH_LOG(info, TAG, "WiFi configuration not found, starting AP...");
      initializeAP();
    }
  }
}

void ESP32Connectivity::initializeWiFiStack() {
  // set netif
  esp_netif_t* sta_netif = esp_netif_create_default_wifi_sta();
  assert(sta_netif);

  // initialize with default configuration
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // Initialize event handler
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &wifiEventHandler, this, NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &wifiEventHandler, this, NULL));
}

void ESP32Connectivity::initializeAP() {
  // Setup configuration for AP mode
  wifi_config_t apConfig = {};
  memset(&apConfig, 0, sizeof(apConfig));

  // Setup configuration for STA mode
  wifi_config_t staConfig = {};
  memset(&staConfig, 0, sizeof(apConfig));

  // Make it open, 4 max connections
  apConfig.ap.channel = 1;
  apConfig.ap.max_connection = 4;
  apConfig.ap.authmode = WIFI_AUTH_OPEN;

  staConfig.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
  staConfig.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;
  staConfig.sta.threshold.rssi = -127;
  staConfig.sta.threshold.authmode = WIFI_AUTH_OPEN;

  std::string apSsid = "Euphonium";
  std::string apPasswd = "";

  // set AP's credentials
  strcpy((char*)apConfig.ap.ssid, apSsid.c_str());
  strcpy((char*)apConfig.ap.password, apPasswd.c_str());
  apConfig.ap.ssid_len = apSsid.size();

  esp_wifi_stop();
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &apConfig));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &staConfig));

  ESP_ERROR_CHECK(esp_wifi_start());
}

void ESP32Connectivity::handleEvent(esp_event_base_t event_base,
                                    int32_t event_id, void* event_data) {
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_START) {
    // AP Mode has started successfully - let the rest of the system know.
    this->data.state = State::CONNECTED_NO_INTERNET;
    this->data.type = ConnectivityType::WIFI_AP;

    EUPH_LOG(debug, TAG, "Sending state update...");
    // Update
    this->sendStateUpdate();
  }
}

void ESP32Connectivity::registerHandlers(
    std::shared_ptr<bell::BellHTTPServer> http) {}
