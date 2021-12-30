#include "WiFiDriver.h"

WiFiState globalWiFiState;
wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];

void publishWiFiEvent(std::string eventType, berry::map networks = berry::map(),
                      std::string ipAddr = "") {
    auto event =
        std::make_unique<WiFiStateChangedEvent>(eventType, networks, ipAddr);
    mainEventBus->postEvent(std::move(event));
}

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        std::scoped_lock lock(globalWiFiState.stateMutex);
        if (globalWiFiState.isConnecting) {
            esp_wifi_connect();
            publishWiFiEvent("connecting");
        }
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_START) {
        std::scoped_lock lock(globalWiFiState.stateMutex);
        if (!globalWiFiState.isConnecting) {
            publishWiFiEvent("ap_ready");
            //start_dns_server();
        }
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_SCAN_DONE) {
        uint16_t number = DEFAULT_SCAN_LIST_SIZE;
        uint16_t ap_count = 0;
        memset(ap_info, 0, sizeof(ap_info));

        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));

        // iterate over networks and save to berry map
        auto networks = berry::map();

        for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {
            networks.insert(
                {std::string(reinterpret_cast<const char *>(ap_info[i].ssid)),
                 berry::map{
                     {"rssi", ap_info[i].rssi},
                     {"open", ap_info[i].authmode == WIFI_AUTH_OPEN},
                 }});
        }
        publishWiFiEvent("scan_done", networks);
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        std::scoped_lock lock(globalWiFiState.stateMutex);
        if (globalWiFiState.isConnecting) {
            if (globalWiFiState.reconnectCount < MAX_RECONNECT_COUNT) {
                BELL_LOG(error, "wifi", "Retry %d",
                         (globalWiFiState.reconnectCount + 1));
                globalWiFiState.isConnecting = true;
                globalWiFiState.reconnectCount++;
                esp_wifi_connect();
            } else {
                globalWiFiState.isConnecting = false;
                globalWiFiState.reconnectCount = 0;
                BELL_LOG(error, "wifi", "Can't connect");
                publishWiFiEvent("no_ap");
            }
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        std::scoped_lock lock(globalWiFiState.stateMutex);
        globalWiFiState.connected = true;

        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        char strIp[16];
        esp_ip4addr_ntoa(&event->ip_info.ip, strIp, IP4ADDR_STRLEN_MAX);

        BELL_LOG(info, "wifi", "Connected, ip addr: %s", strIp);
        publishWiFiEvent("connected", berry::map(), std::string(strIp));
        // setup mdns
        mdns_init();
        mdns_hostname_set("cspot");
    }
}

void initializeWiFiStack() {

    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
    assert(ap_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));
}

wifi_config_t setWiFiConfigBase(bool ap) {
    wifi_config_t wifi_config = {};
    memset(&wifi_config, 0, sizeof(wifi_config));

    if (!ap) {
        wifi_config.sta.scan_method = WIFI_FAST_SCAN;
        wifi_config.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;
        wifi_config.sta.threshold.rssi = -127;
        wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
    } else {
        wifi_config.ap.channel = 1;
        wifi_config.ap.max_connection = 4;
        wifi_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    }
    return wifi_config;
}

void tryToConnect(std::string ssid, std::string password, bool fromAp) {
    BELL_LOG(info, "wifi", "Connecting to WiFi %s / %s", ssid.c_str(),
             password.c_str());

    // Initialize and start WiFi
    wifi_config_t wifi_config = setWiFiConfigBase(false);
    globalWiFiState.reconnectCount = 0;
    globalWiFiState.isConnecting = true;

    strcpy(reinterpret_cast<char *>(wifi_config.sta.ssid), ssid.c_str());
    strcpy(reinterpret_cast<char *>(wifi_config.sta.password),
           password.c_str());

    if (!fromAp) {
        esp_wifi_stop();
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());
    } else {
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_connect());
    }
}

void startFastScan() {
    std::scoped_lock lock(globalWiFiState.stateMutex);
    if (!globalWiFiState.connected && !globalWiFiState.isConnecting) {
        ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));
    }
}

void setupAP(std::string ssid, std::string password) {
    // Initialize and start WiFi
    wifi_config_t wifi_config = setWiFiConfigBase(true);
    wifi_config_t wifi_config_sta = setWiFiConfigBase(false);

    strcpy(reinterpret_cast<char *>(wifi_config.ap.ssid), ssid.c_str());
    strcpy(reinterpret_cast<char *>(wifi_config.ap.password), password.c_str());
    wifi_config.ap.ssid_len = ssid.size();
    globalWiFiState.isConnecting = false;

    esp_wifi_stop();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config_sta));

    ESP_ERROR_CHECK(esp_wifi_start());
}

void exportWiFiDriver(std::shared_ptr<berry::VmState> berry) {
    berry->export_function("wifi_init", &initializeWiFiStack);
    berry->export_function("wifi_connect", &tryToConnect);
    berry->export_function("wifi_start_ap", &setupAP);
    berry->export_function("wifi_start_scan", &startFastScan);
}
