#pragma once

#include <stddef.h>
#include <stdint.h>
#include <exception>
#include <string>
#include "mbedtls/sha256.h"

namespace euph {

class FirmwareImageUpdaterException : public std::exception {
 public:
  FirmwareImageUpdaterException(const char* message) : message(message) {}
  const char* what() const noexcept override { return message; }

 private:
  const char* message;
};

/**
* @brief Abstract class describing the interface for updating the firmware image binary.
* 
* As this behaviour is platform-specific, this class should be implemented by the platform-specific code.
*
* The implementations of this class shall begin an OTA transaction upon construction and
* write data to the inactive OTA A/B partition. The implementation shall support finalizing and
* aborting the OTA transaction.
*
* Each instance of this class shall be used only once.
*/
class FirmwareImageUpdater {
 public:
  FirmwareImageUpdater();
  virtual ~FirmwareImageUpdater() = default;
  /**
     * @brief Write incoming data to the inactive OTA partition.
     * 
     * @param data The data buffer to write.
     * @param length The length of the data buffer.
     * @throw FirmwareImageUpdaterException if the OTA data cannot be written.
     */

  virtual void writeData(const uint8_t* data, size_t length) = 0;
  /**
     * @brief Finish the OTA transaction and switch to the updated firmware.
     * A reboot is expected after this call.
     * @throw FirmwareImageUpdaterException if the OTA transaction cannot be finalized.
     */
  virtual void finalize() = 0;

  /**
     * @brief Abort the OTA transaction.
     * Do not switch to the updated firmware (the partition can be clobbered if this is called after writeData()).
     * @throw FirmwareImageUpdaterException if the OTA transaction cannot be aborted.
     */
  virtual void abort() = 0;

  /**
     * @brief Verify the SHA-256 hash of the firmware image.
     * Shall be called after all data has been written.
     * 
     * @param expectedHash The expected SHA-256 hash of the firmware image.
     * @throw FirmwareImageUpdaterException if the hash does not match the expected hash.
     */
  void sha256Verify(const std::string& expectedHash);

 protected:
  mbedtls_sha256_context sha256Context;
  /**
     * @brief Update the SHA-256 hash of the firmware image with new data.
     * Shall be called by the writeData() implementation.
     * 
     * @param data The data buffer to hash
     * @param length The length of the data buffer
     */
  void sha256Update(const uint8_t* data, size_t length);
};
}  // namespace euph
