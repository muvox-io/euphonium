#include "FirmwareImageUpdater.h"
#include <fmt/core.h>
#include <cstdio>

using namespace euph;

FirmwareImageUpdater::FirmwareImageUpdater() {
  mbedtls_sha256_init(&sha256Context);
}

void FirmwareImageUpdater::writeData(const uint8_t* data, size_t length) {
  mbedtls_sha256_update(&sha256Context, data, length);
}

void FirmwareImageUpdater::sha256Update(const uint8_t* data, size_t length) {
  mbedtls_sha256_update(&sha256Context, data, length);
}

void FirmwareImageUpdater::sha256Verify(const std::string& expectedHash) {
  unsigned char hash[32];
  mbedtls_sha256_finish(&sha256Context, hash);
  mbedtls_sha256_free(&sha256Context);

  std::string actualHash;
  for (int i = 0; i < sizeof(hash); i++) {
    char buf[3];
    snprintf(buf, sizeof(buf), "%02x", hash[i]);
    actualHash += buf;
  }

  if (actualHash != expectedHash) {
    throw FirmwareImageUpdaterException(
        fmt::format("SHA-256 hash mismatch: expected {}, got {}", expectedHash,
                    actualHash)
            .c_str());
  }
}
