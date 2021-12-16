#include "ota.h"

void download_ota() {
    char *buffer = malloc(512 + 1);
    esp_http_client_config_t config = {
        .url = "https://t.aa4.eu/euphonium-esp32.bin",
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err;
    if ((err = esp_http_client_open(client, 0)) != ESP_OK) {
        ESP_LOGE("euph_boot", "Failed to open HTTP connection: %s",
                 esp_err_to_name(err));
        free(buffer);
        return;
    }
    int content_length = esp_http_client_fetch_headers(client);
    printf("man %d\n", content_length);
    int total_read_len = 0, read_len = 1;
    
    esp_ota_handle_t ota_handle = 0;
    const esp_partition_t* ota_partition = get_ota_partition_index();
    int re = esp_ota_begin(ota_partition, OTA_WITH_SEQUENTIAL_WRITES, &ota_handle);
    printf("ok %d\n", re);
    while (read_len > 0) {
        read_len = esp_http_client_read(client, buffer, 512);
        if (read_len <= 0) {
            ESP_LOGE("euph_boot", "Error read data");
        }
        ESP_LOGI("euph_boot", "read_len = %d", read_len);
        esp_ota_write(ota_handle, (const void *)buffer, read_len);
    }
    int res = esp_ota_end(ota_handle);

    if (res == ESP_OK) {
        ESP_LOGI("euph_boot", "Update complete successfully");
    }
    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    free(buffer);
}

const esp_partition_t *get_ota_partition_index() {
    esp_partition_iterator_t pi;

    pi = esp_partition_find(ESP_PARTITION_TYPE_APP,
                            ESP_PARTITION_SUBTYPE_APP_OTA_0, "app");
    if (pi == NULL) {
        ESP_LOGE("esp_boot", "Failed to find factory partition");
    }

    const esp_partition_t* factory = esp_partition_get(pi);    // Get partition struct
    esp_partition_iterator_release(pi); // Release the iterator
    return factory;
}

void boot_to_app() {

    esp_err_t err;
    const esp_partition_t *factory = get_ota_partition_index();
    err = esp_ota_set_boot_partition(factory); // Set partition for boot
    if (err != ESP_OK)                         // Check error
    {
        ESP_LOGE("esp_boot", "Failed to set boot partition");
    } else {
        esp_restart(); // Restart ESP
    }
}
