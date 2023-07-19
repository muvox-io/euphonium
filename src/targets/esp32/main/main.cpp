#include <memory>
#include <string>
#include "EuphoniumApp.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

#define MUVOX_SDA 23
#define MUVOX_SCL 22

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

  // TODO: Move this to some kind of platform initialization
  i2c_config_t i2c_config = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = MUVOX_SDA,
      .scl_io_num = MUVOX_SCL,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
  };

  // Install I2C port 0
  i2c_config.master.clk_speed = 100000;
  i2c_param_config(I2C_NUM_0, &i2c_config);
  i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, false, false, false);

  // Start the app
  auto app = new euph::EuphoniumApp();
  vTaskSuspend(NULL);
}
