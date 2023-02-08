#include "ESP32Connectivity.h"
#include "esp_wifi.h"

using namespace euph;

static void wifiEventHandler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data) {
  ESP32Connectivity* self = static_cast<ESP32Connectivity*>(arg);

  // Relay event to the class instance
  self->handleEvent(event_base, event_id, event_data);
}

ESP32Connectivity::ESP32Connectivity(std::shared_ptr<euph::EventBus> eventBus)
    : bell::Task("Connectivity", 4 * 1024, 0, 0) {
  this->eventBus = eventBus;
  this->captivePortalDNS = std::make_unique<euph::CaptivePortalTask>();

  this->data = {
      Connectivity::State::DISCONNECTED,
      Connectivity::ConnectivityType::DEFAULT,
  };

  this->data.jsonBody = "{}";
  this->dataUpdateSemaphore = std::make_unique<bell::WrappedSemaphore>(5);

  this->initializeWiFiStack();
  this->initConfiguration();

  this->startTask();
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
      std::string configStr(wifiConfigSize - 1, ' ');
      handle->get_string(nvsWiFiKey.c_str(), configStr.data(), wifiConfigSize);

      nlohmann::json wifiConfigObj = nlohmann::json::parse(configStr.c_str());

      initializeSTA();
      this->attemptConnect(wifiConfigObj["ssid"], wifiConfigObj["password"]);
    } else {
      EUPH_LOG(info, TAG, "WiFi configuration not found, starting AP...");
      initializeAP();
    }
  }
}

void ESP32Connectivity::persistConfig() {
  esp_err_t err;
  std::unique_ptr<nvs::NVSHandle> handle =
      nvs::open_nvs_handle("storage", NVS_READWRITE, &err);
  if (err != ESP_OK) {
    EUPH_LOG(error, TAG, "Error (%s) opening NVS handle!\n",
             esp_err_to_name(err));
  } else {
    EUPH_LOG(info, TAG, "Reading WiFi configuration from NVS storage...");

    size_t wifiConfigSize = 0;

    nlohmann::json cfgBody = {};
    // Using FMT as nlohmann::json likes to break stack limits on sys_evt task
    cfgBody["ssid"] = this->ssid;
    cfgBody["password"] = this->password;
    std::string cfg = cfgBody.dump();

    handle->set_string(nvsWiFiKey.c_str(), cfg.c_str());
  }
}

void ESP32Connectivity::initializeWiFiStack() {
  // set netif
  esp_netif_t* staNetif = esp_netif_create_default_wifi_sta();
  assert(staNetif);

  esp_netif_t* apNetif = esp_netif_create_default_wifi_ap();
  assert(apNetif);

  // initialize with default configuration
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // Initialize event handler
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &wifiEventHandler, this, NULL));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &wifiEventHandler, this, NULL));

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
}

void ESP32Connectivity::initializeSTA() {
  isApMode = false;
  // Setup configuration for STA mode
  wifi_config_t staConfig = {};
  memset(&staConfig, 0, sizeof(staConfig));

  staConfig.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
  staConfig.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;
  staConfig.sta.threshold.rssi = -127;
  staConfig.sta.threshold.authmode = WIFI_AUTH_OPEN;

  esp_wifi_stop();
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &staConfig));

  ESP_ERROR_CHECK(esp_wifi_start());
}

void ESP32Connectivity::displayNameLoaded(std::string& name) {
  if (this->isApMode) {
    // Stop the captive portal DNS task
    this->captivePortalDNS->stopTask();

    wifi_config_t apConfig = {};
    memset(&apConfig, 0, sizeof(apConfig));
    esp_wifi_get_config(WIFI_IF_AP, &apConfig);

    // Copy the name into the SSID
    strcpy((char*)apConfig.ap.ssid, name.c_str());
    apConfig.ap.ssid_len = name.size();

    esp_wifi_stop();
    // Apply the new configuration
    esp_wifi_set_config(WIFI_IF_AP, &apConfig);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    esp_wifi_start();
  }
}

void ESP32Connectivity::initializeAP() {
  this->jsonBody["error"] = false;

  isApMode = true;
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
  apConfig.ap.ssid_len = apSsid.length();

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
    this->dataUpdateSemaphore->give();

    this->captivePortalDNS->startTask();
  } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_SCAN_DONE) {
    // Scan finished
    uint16_t number = DEFAULT_SCAN_LIST_SIZE;
    uint16_t apCount = 0;
    memset(scanInfo, 0, sizeof(scanInfo));

    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, scanInfo));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&apCount));

    jsonBody["networks"] = nlohmann::json::array();

    for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < apCount); i++) {
      auto network = scanInfo[i];
      EUPH_LOG(info, TAG, "Got network %s", (char*)scanInfo[i].ssid);
      std::string networkSsid((char*)network.ssid);

      // Add a JSON structure of the network to the list
      jsonBody["networks"].push_back({
          {"ssid", networkSsid},
          {"rssi", network.rssi},
          {"open", network.authmode == WIFI_AUTH_OPEN},
      });
    }

    this->jsonBody["scanning"] = false;

    this->dataUpdateSemaphore->give();

    this->isScanning = false;
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    // Retrieve IP address
    ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
    char strIp[16];
    esp_ip4addr_ntoa(&event->ip_info.ip, strIp, IP4ADDR_STRLEN_MAX);

    EUPH_LOG(info, TAG, "Connected, ip addr %s", strIp);
    this->captivePortalDNS->stopTask();

    this->data.ipAddr = strIp;
    this->data.type = ConnectivityType::WIFI_STA;
    this->data.state = Connectivity::State::CONNECTED;
    this->jsonBody["error"] = false;

    this->dataUpdateSemaphore->give();
    this->persistConfig();

    // Disable AP mode after successful connection
    if (isApMode) {
      // Give the server a bit to respond
      BELL_SLEEP_MS(2000);

      // Go back to STA mode
      ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
      isApMode = false;
    }
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
    if (this->data.state == Connectivity::State::CONNECTING) {
      if (this->connectionAttempts < MAX_CONNECTION_ATTEMPTS) {
        this->connectionAttempts++;

        EUPH_LOG(error, TAG, "WiFi connection retry, attempt #%d",
                 this->connectionAttempts);
        esp_wifi_connect();
      } else {
        EUPH_LOG(error, TAG, "Connection failed, after %d attempts",
                 this->connectionAttempts);
        this->jsonBody["error"] = true;
        this->data.state = Connectivity::State::CONNECTED_NO_INTERNET;
        this->dataUpdateSemaphore->give();

        if (!isApMode) {
          this->initializeAP();
        }
      }
    }
  }
}

void ESP32Connectivity::requestScan() {
  if (isScanning) {
    EUPH_LOG(debug, TAG, "Skipping scan, as one is already in progress");
    return;
  }

  this->isScanning = true;
  this->jsonBody["scanning"] = true;
  this->dataUpdateSemaphore->give();

  ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, false));
}

void ESP32Connectivity::attemptConnect(const std::string& ssid,
                                       const std::string& passwd) {
  if (this->isScanning) {
    esp_wifi_scan_stop();
    this->isScanning = false;
  }

  wifi_config_t staConfig = {};

  esp_wifi_get_config(WIFI_IF_STA, &staConfig);

  strcpy((char*)staConfig.sta.ssid, ssid.c_str());
  strcpy((char*)staConfig.sta.password, passwd.c_str());

  esp_wifi_set_config(WIFI_IF_STA, &staConfig);

  this->ssid = ssid;
  this->password = passwd;

  this->data.state = euph::Connectivity::State::CONNECTING;
  this->data.ssid = ssid;
  this->dataUpdateSemaphore->give();

  esp_wifi_connect();
}

void ESP32Connectivity::registerHandlers(
    std::shared_ptr<bell::BellHTTPServer> http) {
  http->registerGet("/wifi/scan", [this, &http](struct mg_connection* conn) {
    this->requestScan();

    return http->makeJsonResponse("{ \"status\": \"ok\" }");
  });

  http->registerGet("/wifi/state", [this, &http](struct mg_connection* conn) {
    this->sendStateUpdate();

    return http->makeJsonResponse("{ \"status\": \"ok\" }");
  });

  http->registerPost(
      "/wifi/connect", [this, &http](struct mg_connection* conn) {
        auto connInfo = mg_get_request_info(conn);
        std::vector<uint8_t> body(connInfo->content_length);
        mg_read(conn, body.data(), connInfo->content_length);

        auto bodyJson = nlohmann::json::parse(body.begin(), body.end());

        if (bodyJson.find("ssid") == bodyJson.end() ||
            bodyJson.find("password") == bodyJson.end()) {
          return http->makeJsonResponse("{ \"status\": \"error\" }");
        }

        // Attempt connection to the AP
        this->attemptConnect(bodyJson["ssid"], bodyJson["password"]);

        return http->makeJsonResponse("{ \"status\": \"ok\" }");
      });
}

void ESP32Connectivity::runTask() {
  while (true) {
    this->dataUpdateSemaphore->wait();
    this->data.jsonBody = this->jsonBody.dump();
    this->sendStateUpdate();
  }
}
