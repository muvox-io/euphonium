#include "OTAHandler.h"
#include <cstring>
#include <fstream>
#include <memory>
#include <string_view>
#include "BellHTTPServer.h"
#include "BellTar.h"
#include "civetweb.h"
#include "esp_ota_ops.h"

using namespace euph;

OTAHandler::OTAHandler(std::shared_ptr<euph::Context> ctx) : ctx(ctx) {}

static int fieldFound(const char* key, const char* filename, char* path,
                      size_t pathlen, void* userData) {
  auto context = reinterpret_cast<OTAHandler::OTAUploadContext*>(userData);
  std::string_view keyView(key);

  // OTA package - store it in the filesystem
  if (keyView == "package") {
    context->type = OTAHandler::OTAUploadContext::Type::PACKAGE;
    return MG_FORM_FIELD_STORAGE_GET;
  }

  if (keyView == "app") {
    context->type = OTAHandler::OTAUploadContext::Type::APP;
    return MG_FORM_FIELD_STORAGE_GET;
  }

  if (keyView == "sha1") {
    return MG_FORM_FIELD_STORAGE_GET;
  }

  return MG_FORM_FIELD_STORAGE_SKIP;
}

static int fieldGet(const char* key, const char* value, size_t valuelen,
                    void* userData) {
  auto context = reinterpret_cast<OTAHandler::OTAUploadContext*>(userData);

  std::string_view keyView(key);

  if (keyView == "sha1") {
    context->sha1 = std::string(value, valuelen);
    return MG_FORM_FIELD_HANDLE_NEXT;
  }

  if (context->type == OTAHandler::OTAUploadContext::Type::INVALID) {
    // Invalid type, skip
    return MG_FORM_FIELD_HANDLE_NEXT;
  }

  if (valuelen > 0) {
    if (context->currentSize > MAX_OTA_PACKAGE_SIZE) {
      return MG_FORM_FIELD_HANDLE_ABORT;
    }

    auto euphCtx = context->ctx;

    if (context->type == OTAHandler::OTAUploadContext::Type::PACKAGE) {
      // 257 being the size of the header of a tar file
      size_t requiredBytesToValidate = 256 + sizeof("ustar");

      if (context->currentSize < requiredBytesToValidate &&
          valuelen + context->currentSize > requiredBytesToValidate) {
        // We have enough data to check the magic number
        if (memcmp(&value[257], "ustar", 5) != 0) {
          // Invalid file
          context->type = OTAHandler::OTAUploadContext::Type::INVALID;

          // Abort the upload
          return MG_FORM_FIELD_HANDLE_ABORT;
        }
      }
    }
#ifdef ESP_PLATFORM
    if (context->type == OTAHandler::OTAUploadContext::Type::APP) {
      // OTA update start
      if (context->currentSize == 0) {
        const esp_partition_t* updatePartition =
            esp_ota_get_next_update_partition(NULL);
        assert(updatePartition != NULL);

        auto err = esp_ota_begin(updatePartition, OTA_WITH_SEQUENTIAL_WRITES,
                                 &context->otaUpdateHandle);
        if (err != ESP_OK) {
          esp_ota_abort(context->otaUpdateHandle);

          // Abort the upload
          return MG_FORM_FIELD_HANDLE_ABORT;
        }
      }

      auto err = esp_ota_write(context->otaUpdateHandle, value, valuelen);
      if (err != ESP_OK) {
        esp_ota_abort(context->otaUpdateHandle);

        // Abort the upload
        return MG_FORM_FIELD_HANDLE_ABORT;
      }
    }
#endif

    euphCtx->storage->writeFileBytes(
        euphCtx->rootPath + DEFAULT_PKG_UPDATE_PATH,
        std::vector<uint8_t>(value, value + valuelen),
        context->currentSize > 0);

    context->currentSize += valuelen;
    return MG_FORM_FIELD_HANDLE_GET;
  }

  return MG_FORM_FIELD_HANDLE_NEXT;
}

std::string OTAHandler::validatePackage() {
  // Needs to be ran from the storage task's thread
  this->ctx->storage->executeFromTask([this]() {
    std::ifstream pkgArchive(this->ctx->rootPath + DEFAULT_PKG_UPDATE_PATH,
                             std::ios::binary);
    bell::BellTar::reader tarArchive(pkgArchive);
    // Remove directory if it exists

    tarArchive.extract_all_files(ctx->rootPath + DEFAULT_PKG_EXTRACT_PATH);

    std::ifstream manifestFile(
        ctx->rootPath + DEFAULT_PKG_EXTRACT_PATH + "/manifest.json",
        std::ios::binary);
  });
  // Unpack the package
  ctx->storage->extractTar(ctx->rootPath + DEFAULT_PKG_UPDATE_PATH,
                           ctx->rootPath + DEFAULT_PKG_EXTRACT_PATH);

  // Ensure the manifest exists
  return this->ctx->storage->readFile(ctx->rootPath + DEFAULT_PKG_EXTRACT_PATH +
                                      "/manifest.json");
}

void OTAHandler::registerHandlers(
    std::shared_ptr<bell::BellHTTPServer> server) {
  server->registerPost(
      "/ota/verify_package", [this, &server](struct mg_connection* conn) {
        const struct mg_request_info* req_info = mg_get_request_info(conn);

        auto context = std::make_unique<OTAUploadContext>();

        // Prepare a semaphore to wait for the upload to finish
        context->ctx = this->ctx;

        struct mg_form_data_handler fdh = {fieldFound, fieldGet, NULL,
                                           context.get()};
        (void)req_info;

        mg_printf(conn,
                  "HTTP/1.1 200 OK\r\nContent-Type: "
                  "application/json\r\nConnection: close\r\n\r\n");
        fdh.user_data = context.get();
        int ret = mg_handle_form_request(conn, &fdh);

        if (ret <= 0 || context->type == OTAUploadContext::Type::INVALID) {
          mg_printf(conn, "{\"status\": \"error\"}");
        }
        
#ifdef ESP_PLATFORM
        if (context->type == OTAUploadContext::Type::APP) {
          // OTA update end
        }
#endif

        // Try to extract and read manifest file
        try {
          mg_printf(conn, "%s", this->validatePackage().c_str());
        } catch (std::exception& e) {
          mg_printf(conn, "{\"status\": \"error\"}");
        }

        return server->makeEmptyResponse();
      });
}