#pragma once

typedef enum
{
    SERVER_OK,
    SERVER_FATFS_INIT_FAILED,
    SERVER_FATFS_INFO_FAILED,
    SERVER_FAILED,
} server_res_t;

server_res_t server_init();
void server_deinit();