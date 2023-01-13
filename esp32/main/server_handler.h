#pragma once

#include "esp_http_server.h"
#include "esp_err.h"


esp_err_t rest_common_handler(httpd_req_t *req);
esp_err_t wifi_scan_handler(httpd_req_t *req);