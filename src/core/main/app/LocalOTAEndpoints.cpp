#include "LocalOTAEndpoints.h"
#include <filesystem>
#include <fstream>
#include <memory>
#include <semaphore>
#include "FirmwareImageUpdater.h"
#include "UpdateFilesystemServiceJob.h"
#include "UpdateFirmwareFromUploadServiceJob.h"

using namespace euph;

static const char* TAG = "LocalOTAEndpoints";

/**
 * @brief Helper struct passed as userData to mongoose form data handlers.
 */
struct FilesystemUpdateContext {
  bool hasValidFile;
  bool isCurrentlyReceivingFile;
  std::string tarFilePath;
  std::ofstream file;
};

/**
 * @brief Function to filter the incoming form
 * fields for the filesystem update endpoint.
 * @see mg_form_data_handler
 */
static int filesystemUpdateFieldFound(const char* key, const char* filename,
                                      char* path, size_t pathlen,
                                      void* userData) {

  std::string_view keyView(key);
  FilesystemUpdateContext* context =
      static_cast<FilesystemUpdateContext*>(userData);
  context->isCurrentlyReceivingFile = false;
  if (keyView == "fs") {
    context->hasValidFile = true;
    context->isCurrentlyReceivingFile = true;
    return MG_FORM_FIELD_STORAGE_GET;
  }

  return MG_FORM_FIELD_STORAGE_SKIP;
}

/**
 * @brief Function to handle the incoming form fields for the filesystem update
 * @see mg_form_data_handler
 */
static int filesystemUpdateFieldGet(const char* key, const char* value,
                                    size_t valuelen, void* userData) {

  std::string_view keyView(key);
  FilesystemUpdateContext* context =
      static_cast<FilesystemUpdateContext*>(userData);

  if (context->isCurrentlyReceivingFile) {
    if (!context->file.is_open()) {
      context->file = std::ofstream(context->tarFilePath,
                                    std::ios::binary | std::ios::trunc);
      if (!context->file.is_open()) {
        EUPH_LOG(error, TAG,
                 "Could not open file '/fs/tmp/fs.tar' for writing");
        return MG_FORM_FIELD_HANDLE_ABORT;
      }
    }

    context->file.write(value, valuelen);
    return MG_FORM_FIELD_HANDLE_GET;
  }

  return MG_FORM_FIELD_HANDLE_NEXT;
}

void euph::registerLocalOTAEndpoints(bell::BellHTTPServer& server,
                                     std::weak_ptr<euph::Context> ctxPtr) {
  server.registerPost(
      "/api/emergency-mode/filesystem-update",
      [&server, ctxPtr](struct mg_connection* conn) {
        auto ctx = ctxPtr.lock();
        if (!ctx) {
          EUPH_LOG(error, TAG, "Could not lock context.");
          return server.makeEmptyResponse();
        }

        std::string tmpDir = ctx->rootPath + "/tmp";

        FilesystemUpdateContext context{.hasValidFile = false,
                                        .isCurrentlyReceivingFile = false,
                                        .tarFilePath = tmpDir + "/fs.tar",
                                        .file = std::ofstream()};

        // Check if /tmp exists (might not be the case on boards with completely erased flash)
        if (!std::filesystem::exists(tmpDir)) {

          EUPH_LOG(info, TAG, "Creating %s directory", tmpDir.c_str());
          // Create /tmp
          std::filesystem::create_directory(tmpDir);
        }

        struct mg_form_data_handler fdh = {filesystemUpdateFieldFound,
                                           filesystemUpdateFieldGet, NULL,
                                           &context};

        int ret = mg_handle_form_request(conn, &fdh);
        if (context.file.is_open()) {
          context.file.close();
        }
        if (ret <= 0 || !context.hasValidFile) {
          mg_printf(conn,
                    "HTTP/1.1 400 OK\r\nContent-Type: "
                    "application/json\r\nConnection: close\r\n\r\n");
          mg_printf(conn, "{\"status\": \"error\"}");
          return server.makeEmptyResponse();
        }

        EUPH_LOG(info, TAG,
                 "Filesystem update archive received successfully, unpacking.");
        std::unique_ptr<ServiceJob> updateJob =
            std::make_unique<UpdateFilesystemServiceJob>(context.tarFilePath);

        if (!ctx->serviceTask->submitJob(std::move(updateJob))) {
          EUPH_LOG(error, TAG,
                   "Could not submit filesystem update job to service task.");
        }

        mg_printf(conn,
                  "HTTP/1.1 200 OK\r\nContent-Type: "
                  "application/json\r\nConnection: close\r\n\r\n");
        mg_printf(conn, "{}");

        return server.makeEmptyResponse();
      });

  server.registerPost(
      "/api/emergency-mode/firmware-image-update",
      [&server, ctxPtr](struct mg_connection* conn) {
        auto ctx = ctxPtr.lock();
        if (!ctx) {
          EUPH_LOG(error, TAG, "Could not lock context.");
          return server.makeEmptyResponse();
        }

        if (!ctx->firmwareImageUpdaterFactory) {
          EUPH_LOG(error, TAG, "Firmware image updater factory not set.");
          return server.makeJsonResponse(
              "{\"status\": \"error\", \"message\":\"Firmware image update  "
              "is "
              "not available on this platform.\"}",
              404);
        }

        std::binary_semaphore update_done(0);

        bool job_submitted = ctx->serviceTask->submitJob(
            std::make_unique<UpdateFirmwareFromUploadServiceJob>(conn,
                                                                 update_done));
        EUPH_LOG(info, TAG, "Firmware image update job submitted.");
        update_done
            .acquire();  // Wait for the job to finish (the response will be sent by it)

        EUPH_LOG(info, TAG, "Firmware image update job finished.");

        return server.makeEmptyResponse();
      });
}
