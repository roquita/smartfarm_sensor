#pragma once

typedef enum
{
    WIFI_STATION_OK,
    WIFI_STATION_NVS_FAILED,
    WIFI_STATION_CONFIG_FAILED,
    WIFI_STATION_CONNECT_FAILED,
    WIFI_STATION_ALREADY_INITILIZED,
    WIFI_STATION_UNEXPECTED_FAILED,
} wifi_station_res_t;

wifi_station_res_t wifi_station_init(char *ssid, char *pswd);
void wifi_station_deinit();