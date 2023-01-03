#include <memory>
#include <string>
#include "EuphoniumApp.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

extern "C" {
void app_main(void);
}

void app_main(void) {
  // Initialize non violate storage
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Setup NETIF, main event loop
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  // Start the app
  auto app = new euph::EuphoniumApp();
  vTaskSuspend(NULL);
}
