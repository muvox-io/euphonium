#include "ota.h"
#include "http.h"

int otaTaskHandle = -1;

#define FIRMWARE_RECV_OTA_SIZE (1024 * 2)

static void ota_task(void *pvParameters) {
    euph_ota_manifest_t *ota = load_ota_manifest();
    download_ota(ota);
}

void start_ota_task() {
    if (otaTaskHandle == -1) {
        otaTaskHandle = xTaskCreatePinnedToCore(&ota_task, "ota", 8192, NULL, 5, NULL, 0);
    }
}

void send_file_progress(int file_size, int downloaded_size) {
    char res[400];
    sprintf(res, "{\"progress\": %d, \"total\": %d}", downloaded_size, file_size);

    send_sse_message(res, "ota_progress");
}

void download_ota(euph_ota_manifest_t *manifest) {
    ESP_LOGI("euph_boot", "Getting new firmware...");
    send_ota_state(EUPH_OTA_DOWNLOADING);

    char *buffer = malloc(FIRMWARE_RECV_OTA_SIZE + 1);
    esp_http_client_config_t config = {.disable_auto_redirect = false,
                                       .max_redirection_count = 4,
                                       .buffer_size = 2048,
                                       .buffer_size_tx = 2048};
    config.url = (char *)malloc(strlen(manifest->url) + 1);
    strcpy((char *)config.url, manifest->url);
    ESP_LOGI("euph_boot", "ota_manifest->url = %s", config.url);
    // config.url = manifest->url;
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err;
_handle_conn:
    if ((err = esp_http_client_open(client, 0)) != ESP_OK) {
        ESP_LOGE("euph_boot", "Failed to open HTTP connection: %s",
                 esp_err_to_name(err));
        free(buffer);
        return;
    }
    int content_length = esp_http_client_fetch_headers(client);

    if (content_length > MAX_FIRMWARE_SIZE) {
        ESP_LOGE("euph_boot", "Firmware size exceeded!");
        return;
    }

    int status = esp_http_client_get_status_code(client);
    if (status == 302) {
        int read_len = 1;
        while (read_len > 0) { read_len = esp_http_client_read(client, buffer, FIRMWARE_RECV_OTA_SIZE); }

        esp_http_client_set_redirection(client);
        goto _handle_conn;
    } else if (status != 200) {
        ESP_LOGE("euph_boot", "HTTP status incorrect! %d != 200", status);
        return;
    }

    if (content_length > 0) {
        ESP_LOGI("euph_boot", "Reading firmware of size %d", content_length);
        uint8_t *firmware_buffer = (uint8_t*) malloc(content_length + 1);

        // init mbedtls structures
        mbedtls_platform_setup(NULL);
        unsigned char received_sha256[32];
        mbedtls_md_context_t ctx;
        mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
        mbedtls_md_init(&ctx);
        mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
        mbedtls_md_starts(&ctx);

        int total_read_len = 0, read_len = 0;
        while (total_read_len < content_length) {
            read_len = esp_http_client_read(client, buffer, FIRMWARE_RECV_OTA_SIZE);
            if (read_len <= 0) {
                ESP_LOGE("euph_boot", "Error read data");
            }
            memcpy(firmware_buffer + total_read_len, buffer, read_len);
            mbedtls_md_update(&ctx, (unsigned char *)buffer, read_len);
            total_read_len += read_len;
            send_file_progress(content_length, total_read_len);
        }
        mbedtls_md_finish(&ctx, received_sha256);
        ESP_LOGI("euph_boot", "Received firmware, reading sha256 %d...", total_read_len);

        mbedtls_md_free(&ctx);

        char output[(32 * 2) + 1];

        char *ptr = &output[0];

        int i;

        for (i = 0; i < 32; i++) {
            ptr += sprintf(ptr, "%02x", received_sha256[i]);
        }

        output[64] = '\0';

        if (strcmp(manifest->sha256, output)) {
            ESP_LOGE("euph_boot", 
                    "\nSHA256 of firmware does not match \n got: "
                     "(%s)\nexpected: (%s)!",
                     manifest->sha256, output);
            send_ota_state(EUPH_OTA_SHA256_INVALID);
            free(firmware_buffer);
            return;
        }

        ESP_LOGI("euph_boot", "SHA256 matches!\n");
        esp_ota_handle_t ota_handle = 0;
            
        send_ota_state(EUPH_OTA_FLASHING);
        const esp_partition_t *ota_partition = get_ota_partition_index();
        esp_ota_begin(ota_partition, content_length, &ota_handle);

        esp_ota_write(ota_handle, firmware_buffer, content_length);

        if (esp_ota_end(ota_handle) == ESP_OK) {
            ESP_LOGI("euph_boot", "Update complete successfully");
            send_ota_state(EUPH_OTA_FINISHED);
        } else {
            send_ota_state(EUPH_OTA_ERROR);
        }

        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        free(firmware_buffer);
        free(buffer);

        ota_boot_to_app();
    }
}

const esp_partition_t *get_ota_partition_index() {
    esp_partition_iterator_t pi;

    pi = esp_partition_find(ESP_PARTITION_TYPE_APP,
                            ESP_PARTITION_SUBTYPE_APP_OTA_0, "app");
    if (pi == NULL) {
        ESP_LOGE("esp_boot", "Failed to find factory partition");
    }

    const esp_partition_t *factory = esp_partition_get(pi);
    esp_partition_iterator_release(pi);
    return factory;
}

void ota_boot_to_app() {
    esp_err_t err;
    const esp_partition_t *factory = get_ota_partition_index();
    err = esp_ota_set_boot_partition(factory);
    if (err != ESP_OK)
    {
        ESP_LOGE("esp_boot", "Failed to set boot partition");
    } else {
        esp_restart(); // Restart ESP
    }
}
