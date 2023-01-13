#pragma once

typedef enum
{
    WIFI_CONFIGMODE_OK,
    WIFI_CONFIGMODE_NVS_FAILED,
    WIFI_CONFIGMODE_START_FAILED,
} wifi_configmode_res_t;

wifi_configmode_res_t wifi_configmode_init(char *ssid, char *pswd);
void wifi_configmode_deinit();