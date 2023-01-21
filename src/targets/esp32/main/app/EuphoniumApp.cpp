#include "EuphoniumApp.h"

using namespace euph;

EuphoniumApp::EuphoniumApp() : bell::Task("app", 16 * 1024, 0, 0) {
  initializeEuphoniumLogger();
  initializeStorage();

  this->eventBus = std::make_shared<euph::EventBus>();
  this->connectivity = std::make_shared<ESP32Connectivity>(eventBus);
  this->audioOutput = std::make_shared<euph::I2SAudioOutput>();
  this->statusTask = std::make_shared<euph::StatusLED>(eventBus);
  this->memoryMonitor = std::make_shared<euph::MemoryMonitorTask>();

  this->core = std::make_unique<euph::Core>(connectivity, eventBus, audioOutput);

  this->core->exportPlatformBindings = [this] (std::shared_ptr<euph::Context> ctx) {
    this->core->registerAudioSource(std::make_unique<BluetoothSinkPlugin>(ctx));
    exportDrivers(ctx->vm);
  };

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
  core->handleEventLoop();

  while (true) {
    BELL_SLEEP_MS(100);
  }
}
