#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif_types.h"
#include "esp_partition.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "http.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "nvs_flash.h"
#include "ota.h"
#include "spiffs.h"
#include <esp_ota_ops.h>
#include <string.h>

const char *TAG = "euph_boot";

euph_wifi_config_t *global_wifi_config;
static int s_retry_num = 0;

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI("euph_boot", "Connect WiFi!");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < 5) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            ESP_LOGI("euph_boot", "Wifi fail");
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;

        // start_ota_task();
        register_handlers();
    }
}

void wifi_init_sta(void) {
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .sta = {},
    };

    strcpy((char *)wifi_config.sta.ssid, global_wifi_config->ssid);
    strcpy((char *)wifi_config.sta.password, global_wifi_config->password);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");
}

void app_main(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    esp_wifi_set_ps(WIFI_PS_NONE);

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI("euph_boot", "Loaded recovery");
    spiffs_init();

    if (spiffs_file_exists("/spiffs/ota_manifest.json")) {
        global_wifi_config = load_wifi_config();

        if (global_wifi_config == NULL) {
            ESP_LOGI("euph_boot", "No wifi config found, booting to app");
            // bootToApp();
            ota_boot_to_app();
        }

        ESP_LOGI("euph_boot", "Connecting to wifi");
        wifi_init_sta();
    } else {
        ESP_LOGI("euph_boot", "No ota manifest, booting to app");
        ota_boot_to_app();
    }
    // bootToApp();
}
