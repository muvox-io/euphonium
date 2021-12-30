#include "BluetoothPlugin.h"
#include "Core.h"
#include "DACAudioOutput.h"
#include "ESP32Platform.h"
#include "EuphoniumLog.h"
#include "Logger.h"
#include "OTAPlugin.h"
#include "SPIFFSScriptLoader.h"
#include "ScriptsUpdater.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fstream"
#include "mdns.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include <arpa/inet.h>
#include <esp_heap_caps.h>
#include <memory.h>
#include <memory>
#include <stdio.h>
#include <string.h>
#include <string>

void *operator new(std::size_t count) {
    return heap_caps_malloc(count, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
}

void operator delete(void *ptr) noexcept {
    if (ptr)
        free(ptr);
}

static const char *TAG = "euphonium";

extern "C" {
    void app_main(void);
}

static void euphoniumTask(void *pvParameters) {
    initializeEuphoniumLogger();
    bell::createDecoders();

    auto updater = std::make_shared<ScriptsUpdater>();

    if (updater->versionMatches()) {
        auto core = std::make_shared<Core>();
        core->registeredPlugins.push_back(
            std::make_shared<ESP32PlatformPlugin>());
        core->registeredPlugins.push_back(std::make_shared<OTAPlugin>());
        core->registeredPlugins.push_back(std::make_shared<BluetoothPlugin>());
        auto loader = std::make_shared<SPIFFSScriptLoader>();
        auto output = std::make_shared<DACAudioOutput>();
        core->selectAudioOutput(output);
        core->setupBindings();

        core->loadPlugins(loader);
        core->handleScriptingThread();
    } else {
        BELL_LOG(info, "init",
                 "Script version does not match app, updating...");
        updater->update();
    }
}

void init_spiffs() {
    esp_vfs_spiffs_conf_t conf = {.base_path = "/spiffs",
                                  .partition_label = NULL,
                                  .max_files = 30,
                                  .format_if_mount_failed = true};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    bell::setDefaultLogger();

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)",
                     esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)",
                 esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
}

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_wifi_set_ps(WIFI_PS_NONE);
    init_spiffs();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    auto taskHandle = xTaskCreatePinnedToCore(&euphoniumTask, "euphonium", 1024 * 6,
                                              NULL, 5, NULL, 0);
}
