#ifndef EUPH_RECOVERY_HTTP_H
#define EUPH_RECOVERY_HTTP_H

#include "esp_http_server.h"
#include <sys/param.h>
#include "esp_err.h"
#include <sys/stat.h>
#include "string.h"
#include <sys/socket.h>
#include "esp_log.h"

enum OTA_PROGRESS {
    EUPH_OTA_WAITING = 0,
    EUPH_OTA_DOWNLOADING = 1,
    EUPH_OTA_FLASHING = 2,
    EUPH_OTA_SHA256_INVALID = 3,
    EUPH_OTA_ERROR = 4,
    EUPH_OTA_FINISHED = 5
};

void register_handlers();
void send_sse_message();
void send_ota_state(int);

#endif
