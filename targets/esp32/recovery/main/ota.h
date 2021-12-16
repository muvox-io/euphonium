#ifndef EUPH_RECOVERY_OTA_H
#define EUPH_RECOVERY_OTA_H

#include "esp_log.h"
#include "esp_http_client.h"
#include <esp_ota_ops.h>
#include "esp_partition.h"

const esp_partition_t* get_ota_partition_index();
void download_ota();
void ota_boot_to_app();

#endif
