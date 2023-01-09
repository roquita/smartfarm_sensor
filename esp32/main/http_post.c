
#include "http_post.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_tls.h"

static const char *TAG_HTTP_POST = "HTTP_POST";

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG_HTTP_POST, "HTTP_EVENT_DISCONNECTED");
        int mbedtls_err = 0;
        esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
        if (err != 0)
        {
            ESP_LOGI(TAG_HTTP_POST, "Last esp error code: 0x%x", err);
            ESP_LOGI(TAG_HTTP_POST, "Last mbedtls failure: 0x%x", mbedtls_err);
        }
        break;
    default:
        break;
    }
    return ESP_OK;
}

http_post_res_t http_post(char *host, char *path, char *port, char *payload)
{
    // char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};
    /**
     * NOTE: All the configuration parameters for http_client must be spefied either in URL or as host and path parameters.
     * If host and path parameters are not set, query parameter will be ignored. In such cases,
     * query parameter should be specified in URL.
     *
     * If URL as well as host and path parameters are specified, values of host and path will be considered.
     */
    esp_http_client_config_t config = {
        .host = host,
        .path = path,
        .port = atoi(port),
        .query = "esp32",
        .event_handler = _http_event_handler,
        //.user_data = local_response_buffer,
        .disable_auto_redirect = true,
        //.keep_alive_enable = false,
        //.timeout_ms =0,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // POST
    // const char *post_data = "{\"field1\":\"value1\"}";
    // esp_http_client_set_url(client, "http://httpbin.org/post");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, payload, strlen(payload));
    esp_err_t err = esp_http_client_perform(client);

    http_post_res_t post_res;

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG_HTTP_POST, "HTTP POST request failed: %s", esp_err_to_name(err));

        post_res.tx_err = HTTP_POST_FAILED;
        post_res.status_code = -1;

        return post_res;
    }

    int status_code = esp_http_client_get_status_code(client);
    int64_t content_length = esp_http_client_get_content_length(client);

    ESP_LOGI(TAG_HTTP_POST, "HTTP POST Status = %d, content_length = %lld",
             status_code,
             content_length);

    esp_http_client_cleanup(client);

    post_res.tx_err = HTTP_POST_OK;
    post_res.status_code = status_code;

    return post_res;
}