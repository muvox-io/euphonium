#pragma once

#include <memory>

#include "BellHTTPServer.h"
#include "EuphContext.h"

#ifdef ESP_PLATFORM
#include "esp_ota_ops.h"
#include "esp_partition.h"
#endif

namespace euph {
constexpr size_t MAX_OTA_PACKAGE_SIZE = 1024 * 1024 * 10;
constexpr size_t MAX_OTA_BUFFER_SIZE = 1024 * 32;
constexpr auto DEFAULT_PKG_UPDATE_PATH = "/tmp/pkg.tar";
constexpr auto DEFAULT_PKG_EXTRACT_PATH = "/tmp/pkg";

/**
 * @brief OTAHandler handles all OTA related requests, both for packages and the app itself
 */
class OTAHandler {
 private:
  std::shared_ptr<euph::Context> ctx;
  std::string TAG = "OTAHandler";

 public:
  OTAHandler(std::shared_ptr<euph::Context> ctx);

  struct OTAUploadContext {
    enum class Type { APP, PACKAGE, INVALID };

    std::string sha1;
    Type type = Type::INVALID;
    size_t currentSize = 0;
    std::vector<uint8_t> otaBuffer;

#ifdef ESP_PLATFORM
    esp_ota_handle_t otaUpdateHandle = 0;
#endif

    // Euphonium's context
    std::shared_ptr<euph::Context> ctx;
  };

  void initialize(std::shared_ptr<bell::BellHTTPServer>);

  std::string validatePackage();
};
}  // namespace euph
