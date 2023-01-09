#include "nonvolatile.h"
#include "nvs_flash.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG_NVS = "nonvolatile";
bool is_nvs_initialized = false;

bool nvs_init()
{
    if (is_nvs_initialized)
    {
        ESP_LOGW(TAG_NVS, "%s line %d", __FILE__, __LINE__);
        return true;
    }

    esp_err_t ret = nvs_flash_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG_NVS, "%s line %d", __FILE__, __LINE__);
        is_nvs_initialized = false;
        return false;
    }

    is_nvs_initialized = true;
    return true;
}

void nvs_deinit()
{
    esp_err_t ret = nvs_flash_deinit();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG_NVS, "%s line %d", __FILE__, __LINE__);      
    }
}