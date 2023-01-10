#include "server.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_spiffs.h"
#include "esp_http_server.h"
#include "esp_chip_info.h"
#include "project_defines.h"
#include "server_handler.h"

static const char *TAG_SERVER = "SERVER";

httpd_handle_t server = NULL;

server_res_t server_init()
{
    // INIT FLASH PARTITION FOR SERVER USES
    esp_vfs_spiffs_conf_t conf = {
        .base_path = SPIFFS_DISK_BASE_PATH,
        .partition_label = NULL,
        .max_files = SPIFFS_MAX_FILES,
        .format_if_mount_failed = false};
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG_SERVER, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG_SERVER, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG_SERVER, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return SERVER_FATFS_INIT_FAILED;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG_SERVER, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return SERVER_FATFS_INFO_FAILED;
    }

    ESP_LOGI(TAG_SERVER, "Partition size: total: %d, used: %d", total, used);

    // INIT SERVER ITSELF AND SERVER HANDLERS

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(TAG_SERVER, "Starting HTTP Server");
    httpd_start(&server, &config);
    /*
        httpd_uri_t max7219_post_uri = {
            .uri = "/max7219",
            .method = HTTP_POST,
            .handler = max7219_post_handler,
        };
        httpd_register_uri_handler(server, &max7219_post_uri);
    */
    httpd_uri_t rest_common = {
        .uri = REST_COMMON_URI,
        .method = REST_COMMON_METHOD,
        .handler = rest_common_handler,
    };
    httpd_register_uri_handler(server, &rest_common);

    return SERVER_OK;
}
void server_deinit()
{

    httpd_unregister_uri_handler(server, REST_COMMON_URI, REST_COMMON_METHOD);
    httpd_stop(server);
    esp_vfs_spiffs_unregister(SPIFFS_PARTITION_NAME);
}
