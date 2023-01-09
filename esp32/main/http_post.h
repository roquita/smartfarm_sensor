#pragma once

typedef enum
{
    HTTP_POST_FAILED,
    HTTP_POST_OK,
} http_post_tx_err_t;

typedef struct
{
    http_post_tx_err_t tx_err;
    int status_code;
} http_post_res_t;

http_post_res_t http_post(char *host, char *path, char *port, char *payload);