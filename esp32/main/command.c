#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "nonvolatile.h"
#include "wifi_station.h"
#include "project_defines.h"
#include "http_post.h"
#include "wifi_accesspoint.h"
#include "server.h"

static const char *TAG_COMMANDS = "COMMANDS";

void command_handler(char *command)
{
    // PARSE COMMAND AT AND ARGS

    // variables
    char *at;
    char *args[COMMAND_NUM_ARGS_MAX] = {0};
    int num_args = 0;

    //  get at
    at = strtok(command, SPLIT_PATTERN);

    // get args
    int index_args = 0;
    while (index_args < COMMAND_NUM_ARGS_MAX)
    {
        char *p = strtok(NULL, SPLIT_PATTERN);
        if (p == NULL)
        {
            break;
        }
        else
        {
            args[index_args] = p;
            index_args++;
            num_args++;
        }
    }

    /*
     ██╗███╗   ██╗██╗████████╗    ███████╗████████╗ █████╗ ████████╗██╗ ██████╗ ███╗   ██╗
     ██║████╗  ██║██║╚══██╔══╝    ██╔════╝╚══██╔══╝██╔══██╗╚══██╔══╝██║██╔═══██╗████╗  ██║
     ██║██╔██╗ ██║██║   ██║       ███████╗   ██║   ███████║   ██║   ██║██║   ██║██╔██╗ ██║
     ██║██║╚██╗██║██║   ██║       ╚════██║   ██║   ██╔══██║   ██║   ██║██║   ██║██║╚██╗██║
     ██║██║ ╚████║██║   ██║       ███████║   ██║   ██║  ██║   ██║   ██║╚██████╔╝██║ ╚████║
     ╚═╝╚═╝  ╚═══╝╚═╝   ╚═╝       ╚══════╝   ╚═╝   ╚═╝  ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝
    */
    bool command_is_InitStation = (strcmp(at, AT_INIT_STATION) == 0) && (num_args == 2);
    if (command_is_InitStation)
    {
        ESP_LOGI(TAG_COMMANDS, AT_INIT_STATION);

        // EXECUTE
        char *ssid = args[0];
        char *pswd = args[1];
        wifi_station_res_t res = wifi_station_init(ssid, pswd);

        // ANSWER TO MASTER
        if (res == WIFI_STATION_OK)
        {
            uart_write_bytes(UART_NUM, COMMAND_INIT_STATION_OK, strlen(COMMAND_INIT_STATION_OK));
        }
        else
        {
            uart_write_bytes(UART_NUM, COMMAND_INIT_STATION_FAILED, strlen(COMMAND_INIT_STATION_FAILED));
        }
        return;
    }
    /*

     ██████╗ ███████╗██╗███╗   ██╗██╗████████╗    ███████╗████████╗ █████╗ ████████╗██╗ ██████╗ ███╗   ██╗
     ██╔══██╗██╔════╝██║████╗  ██║██║╚══██╔══╝    ██╔════╝╚══██╔══╝██╔══██╗╚══██╔══╝██║██╔═══██╗████╗  ██║
     ██║  ██║█████╗  ██║██╔██╗ ██║██║   ██║       ███████╗   ██║   ███████║   ██║   ██║██║   ██║██╔██╗ ██║
     ██║  ██║██╔══╝  ██║██║╚██╗██║██║   ██║       ╚════██║   ██║   ██╔══██║   ██║   ██║██║   ██║██║╚██╗██║
     ██████╔╝███████╗██║██║ ╚████║██║   ██║       ███████║   ██║   ██║  ██║   ██║   ██║╚██████╔╝██║ ╚████║
     ╚═════╝ ╚══════╝╚═╝╚═╝  ╚═══╝╚═╝   ╚═╝       ╚══════╝   ╚═╝   ╚═╝  ╚═╝   ╚═╝   ╚═╝ ╚═════╝ ╚═╝  ╚═══╝


    */
    bool command_is_DeinitStation = (strcmp(at, AT_DEINIT_STATION) == 0) && (num_args == 0);
    if (command_is_DeinitStation)
    {
        ESP_LOGI(TAG_COMMANDS, AT_DEINIT_STATION);

        // EXECUTE
        wifi_station_deinit();

        // ANSWER TO MASTER
        uart_write_bytes(UART_NUM, COMMAND_DEINIT_STATION_OK, strlen(COMMAND_DEINIT_STATION_OK));
        return;
    }
    /*

     ██╗  ██╗████████╗████████╗██████╗     ██████╗  ██████╗ ███████╗████████╗
     ██║  ██║╚══██╔══╝╚══██╔══╝██╔══██╗    ██╔══██╗██╔═══██╗██╔════╝╚══██╔══╝
     ███████║   ██║      ██║   ██████╔╝    ██████╔╝██║   ██║███████╗   ██║
     ██╔══██║   ██║      ██║   ██╔═══╝     ██╔═══╝ ██║   ██║╚════██║   ██║
     ██║  ██║   ██║      ██║   ██║         ██║     ╚██████╔╝███████║   ██║
     ╚═╝  ╚═╝   ╚═╝      ╚═╝   ╚═╝         ╚═╝      ╚═════╝ ╚══════╝   ╚═╝


    */
    bool command_is_HttpPost = (strcmp(at, AT_HTTP_POST) == 0) && (num_args == 4);
    if (command_is_HttpPost)
    {
        ESP_LOGI(TAG_COMMANDS, AT_HTTP_POST);

        // EXECUTE
        char *host = args[0];
        char *path = args[1];
        char *port = args[2];
        char *payload = args[3];
        http_post_res_t res = http_post(host, path, port, payload);

        // ANSWER TO MASTER
        if (res.tx_err == HTTP_POST_OK)
        {
            char command_httppost_ok[50];
            int n = snprintf(command_httppost_ok, 50, "%s%s%s%s%i%s", AT_HTTP_POST, SPLIT_PATTERN, "OK", SPLIT_PATTERN, res.status_code, END_PATTERN);
            uart_write_bytes(UART_NUM, command_httppost_ok, strlen(command_httppost_ok));

            if (n >= 49)
            {
                ESP_LOGW(TAG_COMMANDS, "%s line %d", __FILE__, __LINE__);
            }
        }
        else
        {
            uart_write_bytes(UART_NUM, COMMAND_HTTP_POST_FAILED, strlen(COMMAND_HTTP_POST_FAILED));
        }
        return;
    }

    /*

     ██╗███╗   ██╗██╗████████╗     █████╗  ██████╗ ██████╗███████╗███████╗███████╗    ██████╗  ██████╗ ██╗███╗   ██╗████████╗
     ██║████╗  ██║██║╚══██╔══╝    ██╔══██╗██╔════╝██╔════╝██╔════╝██╔════╝██╔════╝    ██╔══██╗██╔═══██╗██║████╗  ██║╚══██╔══╝
     ██║██╔██╗ ██║██║   ██║       ███████║██║     ██║     █████╗  ███████╗███████╗    ██████╔╝██║   ██║██║██╔██╗ ██║   ██║
     ██║██║╚██╗██║██║   ██║       ██╔══██║██║     ██║     ██╔══╝  ╚════██║╚════██║    ██╔═══╝ ██║   ██║██║██║╚██╗██║   ██║
     ██║██║ ╚████║██║   ██║       ██║  ██║╚██████╗╚██████╗███████╗███████║███████║    ██║     ╚██████╔╝██║██║ ╚████║   ██║
     ╚═╝╚═╝  ╚═══╝╚═╝   ╚═╝       ╚═╝  ╚═╝ ╚═════╝ ╚═════╝╚══════╝╚══════╝╚══════╝    ╚═╝      ╚═════╝ ╚═╝╚═╝  ╚═══╝   ╚═╝


    */
    bool command_is_InitAccessPoint = (strcmp(at, AT_INIT_ACCESSPOINT) == 0) && (num_args == 2);
    if (command_is_InitAccessPoint)
    {
        ESP_LOGI(TAG_COMMANDS, AT_INIT_ACCESSPOINT);

        // EXECUTE
        char *ssid = args[0];
        char *pswd = args[1];
        wifi_accesspoint_res_t res = wifi_accesspoint_init(ssid, pswd);

        // ANSWER TO MASTER
        if (res == WIFI_ACCESSPOINT_OK)
        {
            uart_write_bytes(UART_NUM, COMMAND_INIT_ACCESSPOINT_OK, strlen(COMMAND_INIT_ACCESSPOINT_OK));
        }
        else
        {
            uart_write_bytes(UART_NUM, COMMAND_INIT_ACCESSPOINT_FAILED, strlen(COMMAND_INIT_ACCESSPOINT_FAILED));
        }
        return;
    }
    /*

     ██████╗ ███████╗██╗███╗   ██╗██╗████████╗     █████╗  ██████╗ ██████╗███████╗███████╗███████╗    ██████╗  ██████╗ ██╗███╗   ██╗████████╗
     ██╔══██╗██╔════╝██║████╗  ██║██║╚══██╔══╝    ██╔══██╗██╔════╝██╔════╝██╔════╝██╔════╝██╔════╝    ██╔══██╗██╔═══██╗██║████╗  ██║╚══██╔══╝
     ██║  ██║█████╗  ██║██╔██╗ ██║██║   ██║       ███████║██║     ██║     █████╗  ███████╗███████╗    ██████╔╝██║   ██║██║██╔██╗ ██║   ██║
     ██║  ██║██╔══╝  ██║██║╚██╗██║██║   ██║       ██╔══██║██║     ██║     ██╔══╝  ╚════██║╚════██║    ██╔═══╝ ██║   ██║██║██║╚██╗██║   ██║
     ██████╔╝███████╗██║██║ ╚████║██║   ██║       ██║  ██║╚██████╗╚██████╗███████╗███████║███████║    ██║     ╚██████╔╝██║██║ ╚████║   ██║
     ╚═════╝ ╚══════╝╚═╝╚═╝  ╚═══╝╚═╝   ╚═╝       ╚═╝  ╚═╝ ╚═════╝ ╚═════╝╚══════╝╚══════╝╚══════╝    ╚═╝      ╚═════╝ ╚═╝╚═╝  ╚═══╝   ╚═╝


    */
    bool command_is_DeinitAccessPoint = (strcmp(at, AT_DEINIT_ACCESSPOINT) == 0) && (num_args == 0);
    if (command_is_DeinitAccessPoint)
    {
        ESP_LOGI(TAG_COMMANDS, AT_DEINIT_ACCESSPOINT);

        // EXECUTE
        wifi_accesspoint_deinit();

        // ANSWER TO MASTER
        uart_write_bytes(UART_NUM, COMMAND_DEINIT_ACCESSPOINT_OK, strlen(COMMAND_DEINIT_ACCESSPOINT_OK));

        return;
    }
    /*

     ██╗███╗   ██╗██╗████████╗    ███████╗███████╗██████╗ ██╗   ██╗███████╗██████╗
     ██║████╗  ██║██║╚══██╔══╝    ██╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗
     ██║██╔██╗ ██║██║   ██║       ███████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝
     ██║██║╚██╗██║██║   ██║       ╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗
     ██║██║ ╚████║██║   ██║       ███████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║
     ╚═╝╚═╝  ╚═══╝╚═╝   ╚═╝       ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝


    */
    bool command_is_InitServer = (strcmp(at, AT_INIT_SERVER) == 0) && (num_args == 0);
    if (command_is_InitServer)
    {
        ESP_LOGI(TAG_COMMANDS, AT_INIT_SERVER);

        // EXECUTE
        server_res_t res = server_init();

        // ANSWER TO MASTER
        if (res == SERVER_OK)
        {
            uart_write_bytes(UART_NUM, COMMAND_INIT_SERVER_OK, strlen(COMMAND_INIT_SERVER_OK));
        }
        else
        {
            uart_write_bytes(UART_NUM, COMMAND_INIT_SERVER_FAILED, strlen(COMMAND_INIT_SERVER_FAILED));
        }
        return;
    }
    /*

     ██████╗ ███████╗██╗███╗   ██╗██╗████████╗    ███████╗███████╗██████╗ ██╗   ██╗███████╗██████╗
     ██╔══██╗██╔════╝██║████╗  ██║██║╚══██╔══╝    ██╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗
     ██║  ██║█████╗  ██║██╔██╗ ██║██║   ██║       ███████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝
     ██║  ██║██╔══╝  ██║██║╚██╗██║██║   ██║       ╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗
     ██████╔╝███████╗██║██║ ╚████║██║   ██║       ███████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║
     ╚═════╝ ╚══════╝╚═╝╚═╝  ╚═══╝╚═╝   ╚═╝       ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝


    */
    bool command_is_DeinitServer = (strcmp(at, AT_DEINIT_SERVER) == 0) && (num_args == 0);
    if (command_is_DeinitServer)
    {
        ESP_LOGI(TAG_COMMANDS, AT_DEINIT_SERVER);

        // EXECUTE
        server_deinit();

        // ANSWER TO MASTER
        uart_write_bytes(UART_NUM, COMMAND_DEINIT_SERVER_OK, strlen(COMMAND_DEINIT_SERVER_OK));

        return;
    }
    /*

     ███████╗██████╗ ██████╗  ██████╗ ██████╗      ██████╗ ██████╗ ███╗   ███╗███╗   ███╗ █████╗ ███╗   ██╗██████╗
     ██╔════╝██╔══██╗██╔══██╗██╔═══██╗██╔══██╗    ██╔════╝██╔═══██╗████╗ ████║████╗ ████║██╔══██╗████╗  ██║██╔══██╗
     █████╗  ██████╔╝██████╔╝██║   ██║██████╔╝    ██║     ██║   ██║██╔████╔██║██╔████╔██║███████║██╔██╗ ██║██║  ██║
     ██╔══╝  ██╔══██╗██╔══██╗██║   ██║██╔══██╗    ██║     ██║   ██║██║╚██╔╝██║██║╚██╔╝██║██╔══██║██║╚██╗██║██║  ██║
     ███████╗██║  ██║██║  ██║╚██████╔╝██║  ██║    ╚██████╗╚██████╔╝██║ ╚═╝ ██║██║ ╚═╝ ██║██║  ██║██║ ╚████║██████╔╝
     ╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝     ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚═════╝


    */
    ESP_LOGE(TAG_COMMANDS, "ERROR COMMAND");
    if (at)
        ESP_LOGE(TAG_COMMANDS, "at -> \"%s\"", at);

    for (int i = 0; i < num_args; i++)
    {
        ESP_LOGE(TAG_COMMANDS, "arg[%i] -> \"%s\"", i, args[i]);
    }
}