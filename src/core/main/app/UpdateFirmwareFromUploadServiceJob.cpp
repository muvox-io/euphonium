#include "UpdateFirmwareFromUploadServiceJob.h"

using namespace euph;

const char* TAG = "UpdateFirmwareFromUploadServiceJob";

UpdateFirmwareFromUploadServiceJob::UpdateFirmwareFromUploadServiceJob(
    mg_connection* conn, std::binary_semaphore& done)
    : done(done), conn(conn) {}

std::string UpdateFirmwareFromUploadServiceJob::jobTypeName() {
  return TAG;
}

struct FirmwareImageUpdateContext {
  std::weak_ptr<euph::Context> ctx;
  std::unique_ptr<FirmwareImageUpdater> updater;
  bool hasValidField;
};

/**
 * @brief Function to filter the incoming form
 * fields for the firmware image update endpoint.
 * @see mg_form_data_handler
 */
static int firmwareImageUpdateFieldFound(const char* key, const char* filename,
                                         char* path, size_t pathlen,
                                         void* userData) {
  std::string_view keyView(key);
  FirmwareImageUpdateContext* context =
      static_cast<FirmwareImageUpdateContext*>(userData);
  context->hasValidField = false;
  if (keyView == "firmware") {
    context->hasValidField = true;
    return MG_FORM_FIELD_STORAGE_GET;
  }

  return MG_FORM_FIELD_STORAGE_SKIP;
}

/**
 * @brief Function to handle the incoming form fields for the firmware image update
 * @see mg_form_data_handler
 */
static int firmwareImageUpdateFieldGet(const char* key, const char* value,
                                       size_t valuelen, void* userData) {
  std::string_view keyView(key);
  FirmwareImageUpdateContext* context =
      static_cast<FirmwareImageUpdateContext*>(userData);

  if (context->hasValidField) {
    try {
      if (!context->updater) {
        context->updater = context->ctx.lock()->firmwareImageUpdaterFactory();
      }

      context->updater->writeData(reinterpret_cast<const uint8_t*>(value),
                                  valuelen);
    } catch (const FirmwareImageUpdaterException& e) {
      EUPH_LOG(error, TAG, "Error writing firmware image data: %s", e.what());
      return MG_FORM_FIELD_HANDLE_ABORT;
    }
    return MG_FORM_FIELD_HANDLE_GET;
  }

  return MG_FORM_FIELD_HANDLE_NEXT;
}

void UpdateFirmwareFromUploadServiceJob::run(
    std::shared_ptr<euph::Context> ctx) {
  FirmwareImageUpdateContext context{.ctx = ctx, .updater = nullptr};

  struct mg_form_data_handler fdh = {firmwareImageUpdateFieldFound,
                                     firmwareImageUpdateFieldGet, NULL,
                                     &context};
  int ret = mg_handle_form_request(conn, &fdh);
  if (ret <= 0 || !context.hasValidField || !context.updater) {
    if (context.updater) {
      context.updater->abort();
    }
    mg_printf(conn,
              "HTTP/1.1 400 OK\r\nContent-Type: "
              "application/json\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "{\"status\": \"error\"}");
    done.release();
    return;
  }

  try {
    context.updater->finalize();
  } catch (const FirmwareImageUpdaterException& e) {
    EUPH_LOG(error, TAG, "Error finalizing firmware image update: %s",
             e.what());
    mg_printf(conn,
              "HTTP/1.1 500 OK\r\nContent-Type: "
              "application/json\r\nConnection: close\r\n\r\n");
    mg_printf(conn, "{\"status\": \"error\"}");
    done.release();
    return;
  }

  EUPH_LOG(info, TAG,
           "Firmware image update written and finalized successfully.");
  // TODO: reboot

  mg_printf(conn,
            "HTTP/1.1 200 OK\r\nContent-Type: "
            "application/json\r\nConnection: close\r\n\r\n");
  mg_printf(conn, "{}");
  done.release();
}
