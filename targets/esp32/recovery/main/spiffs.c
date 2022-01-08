#include "spiffs.h"
#include "cJSON.h"

const char* LOG_TAG = "euph_boot";
const char* OTA_MANIFEST_FILE = "/spiffs/ota.config.json";
const char* WIFI_CONFIG_FILE = "/spiffs/wifi.config.json";

// Initializes VFS
void spiffs_init() {
    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "storage",
        .format_if_mount_failed = true,
        .dont_mount = false};

    esp_err_t ret = esp_vfs_littlefs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(LOG_TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(LOG_TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(LOG_TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_littlefs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(LOG_TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(LOG_TAG, "Partition size: total: %d, used: %d", total, used);
    }
}

bool spiffs_file_exists(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        return false;
    }
    fclose(fp);
    return true;
}

char* read_file_str(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, fp);
    fclose(fp);

    string[fsize] = '\0';
    return string;
}

euph_wifi_config_t* load_wifi_config() {
    if (!spiffs_file_exists(WIFI_CONFIG_FILE))
    {
        return NULL;
    }

    char *string = read_file_str(WIFI_CONFIG_FILE);

    cJSON *json = cJSON_Parse(string);
    if (json == NULL)
    {
        ESP_LOGE(LOG_TAG, "Failed to parse JSON");
        return false;
    }

    cJSON *ssid = cJSON_GetObjectItemCaseSensitive(json, "ssid");
    cJSON *password = cJSON_GetObjectItemCaseSensitive(json, "password");

    if (ssid == NULL || password == NULL)
    {
        ESP_LOGE(LOG_TAG, "Failed to parse JSON");
        cJSON_Delete(json);
        return false;
    }

    // copy configuration to the struct
    euph_wifi_config_t *config = malloc(sizeof(euph_wifi_config_t));
    config->ssid = (char *)malloc(strlen(ssid->valuestring)+1);
    config->password = (char *)malloc(strlen(password->valuestring)+1);
    strcpy(config->ssid, ssid->valuestring);
    strcpy(config->password, password->valuestring);

    cJSON_Delete(json);

    return config;
}

euph_ota_manifest_t* load_ota_manifest() {
    if (!spiffs_file_exists(OTA_MANIFEST_FILE))
    {
        return NULL;
    }

    char *string = read_file_str(OTA_MANIFEST_FILE);

    cJSON *json = cJSON_Parse(string);
    if (json == NULL)
    {
        ESP_LOGE(LOG_TAG, "Failed to parse JSON");
        return false;
    }

    cJSON *url = cJSON_GetObjectItemCaseSensitive(json, "url");
    cJSON *sha256 = cJSON_GetObjectItemCaseSensitive(json, "sha256");

    if (url == NULL || sha256 == NULL)
    {
        ESP_LOGE(LOG_TAG, "Failed to parse JSON");
        cJSON_Delete(json);
        return false;
    }

    // copy configuration to the struct
    euph_ota_manifest_t *config = malloc(sizeof(euph_ota_manifest_t));
    config->url = (char *)malloc(strlen(url->valuestring)+1);
    config->sha256 = (char *)malloc(strlen(sha256->valuestring)+1);
    strcpy(config->url, url->valuestring);
    strcpy(config->sha256, sha256->valuestring);

    cJSON_Delete(json);

    return config;
}

bool ota_requested() {
    if (!spiffs_file_exists(OTA_MANIFEST_FILE))
    {
        return false;
    }

    char *manifest = read_file_str(OTA_MANIFEST_FILE);

    return false;
}
