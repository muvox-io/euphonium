#pragma once

#include "FirmwareImageUpdater.h"

#include "esp_ota_ops.h"

namespace euph {

/**
 * @brief ESP32-specific implementation of the FirmwareImageUpdater interface.
 * 
 * This class is responsible for writing the incoming data to the inactive OTA partition
 * and finalizing the OTA transaction.
 */
class ESP32FirmwareImageUpdater : public FirmwareImageUpdater {
 public:
  ESP32FirmwareImageUpdater();
  virtual ~ESP32FirmwareImageUpdater() = default;
  void writeData(const uint8_t* data, size_t length) override;
  void finalize() override;
  void abort() override;

 private:
  size_t bytesWritten = 0;
  esp_ota_handle_t otaHandle;
};
}  // namespace euph
