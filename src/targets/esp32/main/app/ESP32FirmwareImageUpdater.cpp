#include "ESP32FirmwareImageUpdater.h"
#include "EuphLogger.h"
#include "esp_err.h"
#include "fmt/core.h"
using namespace euph;

ESP32FirmwareImageUpdater::ESP32FirmwareImageUpdater()
    : FirmwareImageUpdater() {
  const esp_partition_t* partition = esp_ota_get_next_update_partition(NULL);
  if (partition == NULL) {
    throw FirmwareImageUpdaterException("No OTA partition found for update");
  }
  EUPH_LOG(info, "ESP32FirmwareImageUpdater", "Starting ota with partition %s",
           partition->label);

  esp_err_t err =
      esp_ota_begin(partition, OTA_WITH_SEQUENTIAL_WRITES, &otaHandle);
  if (err != ESP_OK) {
    throw FirmwareImageUpdaterException(
        fmt::format("Failed to start OTA: {}", esp_err_to_name(err)).c_str());
  }
}

void ESP32FirmwareImageUpdater::writeData(const uint8_t* data, size_t length) {
  this->sha256Update(data, length);
  esp_err_t err = esp_ota_write(otaHandle, data, length);
  if (err != ESP_OK) {
    throw FirmwareImageUpdaterException(
        fmt::format("Failed to write OTA data: {}", esp_err_to_name(err))
            .c_str());
  }
  bytesWritten += length;
}

void ESP32FirmwareImageUpdater::finalize() {
  EUPH_LOG(info, "ESP32FirmwareImageUpdater",
           "Finalizing OTA, written %d bytes", bytesWritten);
  esp_err_t err = esp_ota_end(otaHandle);
  if (err != ESP_OK) {
    throw FirmwareImageUpdaterException(
        fmt::format("Failed to finalize OTA: {}", esp_err_to_name(err))
            .c_str());
  }
}

void ESP32FirmwareImageUpdater::abort() {
  EUPH_LOG(info, "ESP32FirmwareImageUpdater",
           "Aborting OTA after writing %d bytes", bytesWritten);
  esp_ota_abort(otaHandle);
}
