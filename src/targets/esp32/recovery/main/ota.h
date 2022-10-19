#ifndef EUPH_RECOVERY_OTA_H
#define EUPH_RECOVERY_OTA_H

#define MAX_FIRMWARE_SIZE (3 * (1024) * (1024))
#include "esp_log.h"
#include "esp_http_client.h"
#include <esp_ota_ops.h>
#include "esp_partition.h"
#include "spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mbedtls/sha256.h"
#include "mbedtls/platform.h"
#include "mbedtls/md.h"
#include "spiffs.h"

const esp_partition_t* get_ota_partition_index();
void download_ota(euph_ota_manifest_t*);
void ota_boot_to_app();
void start_ota_task();

#endif
