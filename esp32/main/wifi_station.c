#include "wifi_station.h"
#include "stdbool.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "project_defines.h"

static const char *TAG_WIFISTATION = "WIFI_STATION";

/* FreeRTOS event group to signal when we are connected*/
EventGroupHandle_t wifi_sta_event_group = NULL;
esp_netif_t *wifi_sta_netif = NULL;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_GOT_IP_BIT BIT0
#define WIFI_DISCONNECTED_BIT BIT1
#define WIFI_LOST_IP_BIT BIT2
#define WIFI_STOP_BIT BIT3

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(TAG_WIFISTATION, "start");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_STOP)
    {
        ESP_LOGI(TAG_WIFISTATION, "stop");
        xEventGroupSetBits(wifi_sta_event_group, WIFI_STOP_BIT);
    
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGI(TAG_WIFISTATION, "dsconnected");
        xEventGroupSetBits(wifi_sta_event_group, WIFI_DISCONNECTED_BIT);
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG_WIFISTATION, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(wifi_sta_event_group, WIFI_GOT_IP_BIT);
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_LOST_IP)
    {
        ESP_LOGI(TAG_WIFISTATION, "lost ip");
        xEventGroupSetBits(wifi_sta_event_group, WIFI_LOST_IP_BIT);
    }
}

wifi_station_res_t wifi_station_init(char *ssid, char *pswd)
{

    // WIFI NON VOLATILE STORAGE
    esp_err_t nvs_init_succesfull = nvs_flash_init();
    if (nvs_init_succesfull != ESP_OK)
    {
        ESP_LOGE(TAG_WIFISTATION, "%s line %d", __FILE__, __LINE__);
        return WIFI_STATION_NVS_FAILED;
    }

    // WIFI RESOURCES AND CONFIG
    wifi_sta_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_sta_netif = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));

    wifi_config_t wifi_config = {};
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, pswd, sizeof(wifi_config.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    if (esp_wifi_start() != ESP_OK)
    {
        ESP_LOGE(TAG_WIFISTATION, "%s line %d", __FILE__, __LINE__);
        return WIFI_STATION_CONFIG_FAILED;
    }

    // WAIT FOR RESULT
    EventBits_t bits = xEventGroupWaitBits(wifi_sta_event_group,
                                           WIFI_GOT_IP_BIT | WIFI_DISCONNECTED_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    // RESULT
    bool wifi_is_connected = (bits & WIFI_GOT_IP_BIT) != 0;
    if (!wifi_is_connected)
    {
        ESP_LOGE(TAG_WIFISTATION, "Failed to connect to SSID:%s, password:%s", ssid, pswd);
        return WIFI_STATION_CONNECT_FAILED;
    }

    ESP_LOGI(TAG_WIFISTATION, "connected to ap SSID:%s password:%s", ssid, pswd);

    return WIFI_STATION_OK;
}

void wifi_station_deinit()
{
   
    esp_wifi_stop();

    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler);
    esp_event_handler_unregister(IP_EVENT, ESP_EVENT_ANY_ID, &event_handler);

    esp_wifi_deinit();

    esp_netif_destroy_default_wifi(wifi_sta_netif);
    wifi_sta_netif = NULL;

    esp_event_loop_delete_default();
    esp_netif_deinit();

    if (wifi_sta_event_group)
    {
        vEventGroupDelete(wifi_sta_event_group);
        wifi_sta_event_group = NULL;
    }

    nvs_flash_deinit();
  
}