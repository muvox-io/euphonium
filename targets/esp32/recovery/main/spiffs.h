#ifndef EUPH_RECOVERY_SPIFFS_H
#define EUPH_RECOVERY_SPIFFS_H

#include "esp_system.h"
#include "esp_littlefs.h"
#include "esp_log.h"
#include "recovery.h"
#include "cJSON.h"
#include <string.h>
#include <stdio.h>

struct euph_wifi_config {
    char* ssid;
    char* password;
};

struct euph_ota_manifest {
    char* url;
    char* sha256;
    bool requested;
};

typedef struct euph_wifi_config euph_wifi_config_t;
typedef struct euph_ota_manifest euph_ota_manifest_t;

void spiffs_init();
euph_wifi_config_t* load_wifi_config();
euph_ota_manifest_t* load_ota_manifest();
bool spiffs_file_exists(const char* filename);
bool ota_requested();


#endif
