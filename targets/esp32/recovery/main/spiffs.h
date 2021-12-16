#ifndef EUPH_RECOVERY_SPIFFS_H
#define EUPH_RECOVERY_SPIFFS_H

#include "esp_system.h"
#include "esp_spiffs.h"
#include "esp_log.h"
#include "recovery.h"
#include "cJSON.h"
#include <string.h>
#include <stdio.h>

struct euph_wifi_config {
    char* ssid;
    char* password;
};

typedef struct euph_wifi_config euph_wifi_config_t;

void spiffs_init();
euph_wifi_config_t* load_wifi_config();
bool ota_requested();


#endif
