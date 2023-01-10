#pragma once

typedef enum
{
    WIFI_ACCESSPOINT_OK,
    WIFI_ACCESSPOINT_NVS_FAILED,
    WIFI_ACCESSPOINT_CONFIG_FAILED,
} wifi_accesspoint_res_t;

wifi_accesspoint_res_t wifi_accesspoint_init(char *ssid, char *pswd);
void wifi_accesspoint_deinit();