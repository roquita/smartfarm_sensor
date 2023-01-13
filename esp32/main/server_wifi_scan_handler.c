#include "server.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_spiffs.h"
#include "esp_http_server.h"
#include "esp_chip_info.h"
#include "project_defines.h"
#include "esp_vfs.h"
#include <fcntl.h>
#include "esp_wifi.h"
#include "cJSON.h"

static const char *TAG_WIFISCAN_HANDLER = "WIFISCAN HANDLER";

esp_err_t wifi_scan_handler(httpd_req_t *req)
{
    esp_err_t res = esp_wifi_scan_start(NULL, true);

    if (res != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Error, status code: 500");
        return ESP_FAIL;
    }

    // scan ssid
    uint16_t number = WIFI_SSID_SCANNED_MAX;
    wifi_ap_record_t ap_info[WIFI_SSID_SCANNED_MAX];
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
    ESP_LOGI(TAG_WIFISCAN_HANDLER, "Total APs retrieved = %u", number);
/*
    for (int i = 0; i < number; i++)
    {
        ESP_LOGI(TAG_WIFISCAN_HANDLER, "SSID \t\t%s", ap_info[i].ssid);
        ESP_LOGI(TAG_WIFISCAN_HANDLER, "RSSI \t\t%d", ap_info[i].rssi);
        ESP_LOGI(TAG_WIFISCAN_HANDLER, "Channel \t\t%d\n", ap_info[i].primary);
    }
    */

    // create json
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "info", "Success!");
    cJSON *array = cJSON_AddArrayToObject(root, "payload");
    for (int i = 0; i < number; i++)
    {
        cJSON *item = cJSON_CreateString((char *)ap_info[i].ssid);
        cJSON_AddItemToArray(array, item);
    }

    char *root_str = cJSON_PrintUnformatted(root);

    // send json
    httpd_resp_send(req, root_str, strlen(root_str));

    // free memory
    free(root);
    free(root_str);

    return ESP_OK;
}