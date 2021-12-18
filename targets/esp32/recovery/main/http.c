#include "http.h"
#include "esp_http_server.h"
#include "ota.h"

#define IS_FILE_EXT(filename, ext) \
    (strcasecmp(&filename[strlen(filename) - sizeof(ext) + 1], ext) == 0)
#define MAX_SSE_CLIENTS 4

volatile int sse_sockets[MAX_SSE_CLIENTS];
static httpd_handle_t server = NULL;

void free_sse_ctx_func(void *ctx)
{
    int client_fd = *((int*) ctx);

    ESP_LOGD("euph_boot", "free_sse_context sse_socket: %d", client_fd);

    for (int i = 0; i < MAX_SSE_CLIENTS; i++) {
        if (sse_sockets[i] == client_fd) {
            sse_sockets[i] = 0;
        }
    } 
    free(ctx);
}

static esp_err_t server_side_event_registration_handler(httpd_req_t *req) {
    int len;
    char buffer[200];
    int i = 0;

    /* Create session's context if not already available */
    if (!req->sess_ctx) {
        for (i = 0; i < MAX_SSE_CLIENTS; i++) {
            if (sse_sockets[i] == 0) {
                req->sess_ctx = malloc(sizeof(int)); 
                req->free_ctx = free_sse_ctx_func; 
                int client_fd = httpd_req_to_sockfd(req);  
                *(int *)req->sess_ctx = client_fd;
                sse_sockets[i] = client_fd;
                ESP_LOGD("euph_boot", "sse_socket: %d slot %d", sse_sockets[i], i);
                break;
            }
        }
        if (i == MAX_SSE_CLIENTS) {
            len = sprintf(buffer, "HTTP/1.1 503 Server Busy\r\nContent-Length: 0\r\n\r\n");
        }
        else {
            len = sprintf(buffer, "HTTP/1.1 200 OK\r\n"
                                    "Connection: Keep-Alive\r\n"
                                    "Content-Type: text/event-stream\r\n"
                                    "Access-Control-Allow-Origin: *\r\n"
                                    "Access-Control-Allow-Methods: GET, POST, DELETE, OPTIONS\r\n"
                                    "Cache-Control: no-cache\r\n\r\n");
        }
    } else {
        // Should never get here?
        len = sprintf(buffer, "HTTP/1.1 400 Session Already Active\r\n\r\n");
    }

    // need to use raw send function, as httpd_resp_send will add Content-Length: 0 which
    // will make the client disconnect
    httpd_send(req, buffer, len);



    return ESP_OK;
}

void send_ota_state(int state) {
    char* status;
    if (state == EUPH_OTA_WAITING) {
        status = "waiting";
    }
    if (state == EUPH_OTA_DOWNLOADING) {
        status = "downloading";
    }

    if (state == EUPH_OTA_FLASHING) {
        status = "flashing";
    }

    if (state == EUPH_OTA_SHA256_INVALID) {
        status = "invalid_sha";
    }

    if (state == EUPH_OTA_ERROR) {
        status = "error";
    }

    if (state == EUPH_OTA_FINISHED) {
        status = "finished";
    }

    char res[256];
    sprintf(res, "{\"status\": \"%s\"}", status);
    send_sse_message(res, "ota_state");
}

void send_sse_message (char* message, char* event) {
    const char *sse_data = "data: ";
    const char *sse_event = "\nevent: ";
    const char *sse_end_message = "\n\n";

    size_t event_len = 0;
    if (event != NULL) {
        event_len = strlen(sse_event) + strlen(event);
    }
    char send_buf[strlen(sse_data) + strlen(message) + event_len + strlen(sse_end_message)];
    strcpy(send_buf, sse_data);
    strcat(send_buf, message);
    if (event != NULL) {
        strcat(send_buf, sse_event);
        strcat(send_buf, event);
    }
    strcat(send_buf, sse_end_message);

    size_t message_len = strlen(send_buf);

    int return_code;
    for (int i = 0; i < MAX_SSE_CLIENTS; i++) {
        if (sse_sockets[i] != 0) {
            return_code = send(sse_sockets[i], send_buf, message_len, 0);
            if (return_code < 0) {
                httpd_sess_trigger_close(server, sse_sockets[i]);
            }
        }
    }
}

// Set HTTP response content type according to file extension
static const char* get_content_type_from_file(const char *filename)
{
    if (IS_FILE_EXT(filename, ".css")) {
        return "text/css";
    } else if (IS_FILE_EXT(filename, ".html")) {
        return "text/html";
    } else if (IS_FILE_EXT(filename, ".js")) {
        return "application/javascript";
    }
    return "text/plain";
}


static const char* get_path_from_uri(char *dest, const char *uri, size_t destsize)
{
    const char* base_path = "/spiffs";
    const size_t base_pathlen = strlen(base_path);
    size_t pathlen = strlen(uri);

    int url_offset = 0;

    printf("uri = %s\n", uri);
    if (strncmp("/web", uri, 4) == 0) {
        url_offset = 4;
    }

    const char *quest = strchr(uri, '?');
    if (quest) {
        pathlen = MIN(pathlen, quest - uri);
    }
    const char *hash = strchr(uri, '#');
    if (hash) {
        pathlen = MIN(pathlen, hash - uri);
    }

    if (base_pathlen + pathlen + 1 > destsize) {
        // Full path string won't fit into destination buffer
        return NULL;
    }

    // Construct full path (base + path)
    strcpy(dest, base_path);
    strlcpy(dest + base_pathlen, uri + url_offset, pathlen - url_offset + 1);

    // Return pointer to path, skipping the base
    return dest + base_pathlen + url_offset;
}

static esp_err_t serve_info(httpd_req_t *req)
{
    const char resp[] = "{\"networkState\": \"recovery\"}";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    send_sse_message("test", "kocz");
    return ESP_OK;
}

static esp_err_t serve_start_ota(httpd_req_t *req) {
    start_ota_task();
    const char resp[] = "{\"status\": \"ok\"}";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t serve_app(httpd_req_t *req)
{
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS");

    if (strcmp(req->uri, "/info") == 0) {
        return serve_info(req);
    }

    if (strcmp(req->uri, "/events") == 0) {
        return server_side_event_registration_handler(req);
    }

    if (strcmp(req->uri, "/start_ota") == 0) {
        return serve_start_ota(req);
    }

    char filepath[512];
    FILE *fd = NULL;
    struct stat file_stat;

    const char *filename = get_path_from_uri(filepath,
                                             req->uri, sizeof(filepath));

    // make / always redirect to index.html
    if (strcmp(filepath, "/spiffs/") == 0) {
        strcpy(filepath + strlen(filepath), "index.html");
    } 

    if (strcmp(filepath, "/spiffs") == 0) {
        strcpy(filepath + strlen(filepath), "/index.html");
    } 

    const char* content_type = get_content_type_from_file(filename);
_check_file:
    if (stat(filepath, &file_stat) == -1) {
        ESP_LOGE("euph_boot", "Failed to stat file : %s", filepath);
        if (!IS_FILE_EXT(filepath, ".gz")) {
            int path_len = strlen(filepath);
            httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
            strcpy(filepath + path_len, ".gz");
            goto _check_file;
        }
        // Respond with 404 Not Found
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File does not exist");
        return ESP_FAIL;
    }
    fd = fopen(filepath, "r");
    if (!fd) {
        ESP_LOGE("euph_boot", "Failed to read existing file : %s", filepath);

        // Respond with 500 Internal Server Error
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    ESP_LOGI("euph_boot", "Sending file : %s...", filename);
    httpd_resp_set_type(req, content_type);

    // Retrieve the pointer to scratch buffer for temporary storage
    char *chunk = malloc(8192); 
    size_t chunksize;
    do {
        // Read file in chunks into the scratch buffer
        chunksize = fread(chunk, 1, 8192, fd);

        if (chunksize > 0) {
            // Send the buffer contents as HTTP response chunk 
            if (httpd_resp_send_chunk(req, chunk, chunksize) != ESP_OK) {
                fclose(fd);
                ESP_LOGE("euph_boot", "File sending failed!");

                // Abort sending file
                httpd_resp_sendstr_chunk(req, NULL);

                // Respond with 500 Internal Server Error
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
               return ESP_FAIL;
           }
        }
        // Keep looping till the whole file is sent
    } while (chunksize != 0);
    free(chunk);

    // Close file after sending complete
    fclose(fd);
    ESP_LOGI("esp_boot", "File sending complete");

    // Respond with an empty chunk to signal HTTP response completion
    httpd_resp_set_hdr(req, "Connection", "close");
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

httpd_uri_t uri_app = {
    .uri = "/*",
    .method = HTTP_GET,
    .handler = serve_app,
    .user_ctx = NULL
};

httpd_uri_t uri_info = {
    .uri = "/info",
    .method = HTTP_GET,
    .handler = serve_info,
    .user_ctx = NULL
};

httpd_uri_t uri_start_ota = {
    .uri = "/start_ota",
    .method = HTTP_GET,
    .handler = serve_start_ota,
    .user_ctx = NULL
};

httpd_uri_t uri_events = {
    .uri = "/events",
    .method = HTTP_GET,
    .handler = server_side_event_registration_handler,
    .user_ctx = NULL
};

void register_handlers() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_app);
        httpd_register_uri_handler(server, &uri_events);
        httpd_register_uri_handler(server, &uri_info);
        httpd_register_uri_handler(server, &uri_start_ota);
    }
}
