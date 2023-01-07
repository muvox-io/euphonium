#include "EuphoniumApp.h"
#include "EventBus.h"


using namespace euph;

EuphoniumApp::EuphoniumApp() : bell::Task("app", 32 * 1024, 0, 0, false) {
  initializeEuphoniumLogger();
  initializeStorage();

  startTask();
}

void EuphoniumApp::initializeStorage() {
  // Initialize LittleFS Storage
  esp_vfs_littlefs_conf_t conf = {
      .base_path = "/fs",
      .partition_label = "storage",
      .format_if_mount_failed = true,
      .dont_mount = false,
  };

  // Use settings defined above to initialize and mount LittleFS filesystem.
  esp_err_t ret = esp_vfs_littlefs_register(&conf);

  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      EUPH_LOG(error, TASK, "Cannot mount or format LittleFS");
    } else if (ret == ESP_ERR_NOT_FOUND) {
      EUPH_LOG(error, TASK, "No LittleFS partition found");
    } else {
      EUPH_LOG(error, TASK, "LittleFS Cannot initialize");
    }
    return;
  }

  EUPH_LOG(info, TASK, "Storage mounted.");
}

void EuphoniumApp::runTask() {
  auto eventBus = std::make_shared<euph::EventBus>();
  auto connectivity = std::make_shared<ESP32Connectivity>(eventBus);

  auto core = std::make_unique<euph::Core>(connectivity, eventBus);

  while (true) {
    BELL_SLEEP_MS(100);
  }
}
