#pragma once

// UART 1 CONFIG DEFINES
#define UART_NUM UART_NUM_1
#define UART_TX_PIN 25
#define UART_RX_PIN 26
#define UART_BAUDRATE 115200
#define UART_DATA_BITS UART_DATA_8_BITS
#define UART_STOP_BITS UART_STOP_BITS_1
#define UART_PARITY UART_PARITY_DISABLE

#define PATTERN_CHR_NUM (3)
#define PATTERN_CHR_TIMEOUT (9)
#define UART_BUF_SIZE (1024)

// COMMAND DEFINES
#define COMMAND_NUM_ARGS_MAX 5
#define END_PATTERN "+++"
#define SPLIT_PATTERN " "

#define AT_INIT_STATION "AT+InitStation"
#define COMMAND_INIT_STATION_OK (AT_INIT_STATION SPLIT_PATTERN "OK" END_PATTERN)
#define COMMAND_INIT_STATION_FAILED (AT_INIT_STATION SPLIT_PATTERN "FAILED" END_PATTERN)

#define AT_DEINIT_STATION "AT+DeinitStation"
#define COMMAND_DEINIT_STATION_OK (AT_DEINIT_STATION SPLIT_PATTERN "OK" END_PATTERN)

#define AT_HTTP_POST "AT+HttpPost"
#define COMMAND_HTTP_POST_FAILED (AT_HTTP_POST SPLIT_PATTERN "FAILED" END_PATTERN)

#define AT_INIT_CONFIGMODE "AT+InitConfigMode"
#define COMMAND_INIT_CONFIGMODE_OK (AT_INIT_CONFIGMODE SPLIT_PATTERN "OK" END_PATTERN)
#define COMMAND_INIT_CONFIGMODE_FAILED (AT_INIT_CONFIGMODE SPLIT_PATTERN "FAILED" END_PATTERN)

#define AT_DEINIT_CONFIGMODE "AT+DeinitConfigMode"
#define COMMAND_DEINIT_CONFIGMODE_OK (AT_DEINIT_CONFIGMODE SPLIT_PATTERN "OK" END_PATTERN)

#define AT_INIT_SERVER "AT+InitServer"
#define COMMAND_INIT_SERVER_OK (AT_INIT_SERVER SPLIT_PATTERN "OK" END_PATTERN)
#define COMMAND_INIT_SERVER_FAILED (AT_INIT_SERVER SPLIT_PATTERN "FAILED" END_PATTERN)

#define AT_DEINIT_SERVER "AT+DeinitServer"
#define COMMAND_DEINIT_SERVER_OK (AT_DEINIT_SERVER SPLIT_PATTERN "OK" END_PATTERN)

// SPIFFS
#define SPIFFS_PARTITION_NAME "www"
#define SPIFFS_DISK_BASE_PATH ("/" SPIFFS_PARTITION_NAME)
#define SPIFFS_MAX_FILES 5

// SERVER HANDLERS
#define REST_COMMON_URI "/*"
#define REST_COMMON_METHOD HTTP_GET

#define WIFI_SCAN_URI "/api/wifiscan"
#define WIFI_SCAN_METHOD HTTP_GET

#define WIFI_SSID_SCANNED_MAX 20