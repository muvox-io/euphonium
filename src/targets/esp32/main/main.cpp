#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "Core.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include <esp_heap_caps.h>
#include <memory>
#include <string>


extern "C" {
void app_main(void);
}

static void euphoniumTask(void *pvParameters) {
}

void init_littlefs() {
}

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);


    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    init_littlefs();
}
