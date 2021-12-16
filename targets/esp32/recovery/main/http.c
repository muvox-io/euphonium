#include "http.h"
#include "esp_http_server.h"


esp_err_t serve_app(httpd_req_t *req) {
    const char resp[] = "hello kocz";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

httpd_uri_t uri_app = {
    .uri = "/*",
    .method = HTTP_GET,
    .handler = serve_app,
    .user_ctx = NULL
};

void register_handlers() {
    printf("Register handlers called\n");
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_app);
    }
}
