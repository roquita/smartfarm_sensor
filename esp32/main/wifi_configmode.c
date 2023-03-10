#include "wifi_configmode.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG_CONFIGMODE = "CONFIGMODE";

esp_netif_t *wifi_ap_netif = NULL;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG_CONFIGMODE, "station " MACSTR " join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG_CONFIGMODE, "station " MACSTR " leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

wifi_configmode_res_t wifi_configmode_init(char *ap_ssid, char *ap_pswd)
{

    // WIFI NON VOLATILE STORAGE
    esp_err_t nvs_init_succesfull = nvs_flash_init();
    if (nvs_init_succesfull != ESP_OK)
    {
        ESP_LOGE(TAG_CONFIGMODE, "%s line %d", __FILE__, __LINE__);
        return WIFI_CONFIGMODE_NVS_FAILED;
    }

    // setup tcp-ip stack
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_ap_netif = esp_netif_create_default_wifi_ap();

    // init wifi driver
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // set event handlers
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));

    // config wifi driver
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

    wifi_config_t config = {
        .ap = {
            .max_connection = 1,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
        },
    };
    strncpy((char *)config.ap.ssid, ap_ssid, sizeof(config.ap.ssid) - 1);
    config.ap.ssid_len = strlen((char *)config.ap.ssid);
    strncpy((char *)config.ap.password, ap_pswd, sizeof(config.ap.password) - 1);

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &config));


    // start wifi driver
    bool was_access_point_created = esp_wifi_start() == ESP_OK;
    if (!was_access_point_created)
    {
        ESP_LOGE(TAG_CONFIGMODE, "%s line %d", __FILE__, __LINE__);
        return WIFI_CONFIGMODE_START_FAILED;
    }

    ESP_LOGI(TAG_CONFIGMODE, "wifi_init_softap finished. SSID:%s password:%s ", ap_ssid, ap_pswd);

    // start wifi scan

    // esp_wifi_scan_start();

    return WIFI_CONFIGMODE_OK;
}

void wifi_configmode_deinit()
{
    esp_wifi_stop();
    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler);

    esp_wifi_deinit();

    esp_netif_destroy_default_wifi(wifi_ap_netif);
    wifi_ap_netif = NULL;

    esp_event_loop_delete_default();
    esp_netif_deinit();

    nvs_flash_deinit();
}