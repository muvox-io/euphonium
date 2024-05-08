#include "LocalOTAEndpoints.h"
#include <filesystem>
#include <fstream>
#include <memory>
#include "UpdateFilesystemServiceJob.h"

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
       
        const struct mg_request_info* req_info = mg_get_request_info(conn);
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
        (void)req_info;

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
}
