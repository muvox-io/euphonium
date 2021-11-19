#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spiffs.h"
#include <string.h>
#include <arpa/inet.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "EuphoniumLog.h"
#include "esp_event.h"
#include "esp_log.h"
#include "fstream"
#include "Logger.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include <string>
#include "SPIFFSScriptLoader.h"
#include "Core.h"
#include "AC101AudioOutput.h"

static const char *TAG = "euphonium";

extern "C"
{
    void app_main(void);
}
static void euphoniumTask(void *pvParameters)
{
    bell::setDefaultLogger();
    bell::enableSubmoduleLogging();
    auto core = std::make_shared<Core>();
    auto loader = std::make_shared<SPIFFSScriptLoader>();
    auto output = std::make_shared<AC101AudioOutput>();
    core->selectAudioOutput(output);
    core->loadPlugins(loader);
}

void init_spiffs()
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 15,
        .format_if_mount_failed = true};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    bell::setDefaultLogger();

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    init_spiffs();

    esp_wifi_set_ps(WIFI_PS_NONE);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    ESP_LOGI("TAG", "Connected to AP, start euphonium");

    auto taskHandle = xTaskCreatePinnedToCore(&euphoniumTask, "euphonium", 8192 * 8, NULL, 5, NULL, 0);
}
