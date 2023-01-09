#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "command.c"
#include "project_defines.h"
#include "esp_heap_caps.h"

static const char *TAG = "uart_events";

/**
 * This example shows how to use the UART driver to handle special UART events.
 *
 * It also reads data from UART0 directly, and echoes it to console.
 *
 * - Port: UART0
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: on
 * - Pin assignment: TxD (default), RxD (default)
 */

static QueueHandle_t uart1_queue;
static TaskHandle_t uart1_handle;

static void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    size_t buffered_size;
    uint8_t *command = (uint8_t *)malloc(UART_BUF_SIZE);
    uint8_t *pattern = (uint8_t *)malloc(PATTERN_CHR_NUM + 1);

    for (;;)
    {
        // Waiting for UART event.
        if (xQueueReceive(uart1_queue, (void *)&event, (TickType_t)portMAX_DELAY))
        {
            bzero(command, UART_BUF_SIZE);
            ESP_LOGI(TAG, "uart[%d] event:", UART_NUM);
            switch (event.type)
            {
            // Event of UART receving data
            /*We'd better handler data event fast, there would be much more data events than
            other types of events. If we take too much time on data event, the queue might
            be full.*/
            case UART_DATA:
                ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                uart_read_bytes(UART_NUM, command, event.size, portMAX_DELAY);
                ESP_LOGI(TAG, "[DATA EVT]:");
                uart_write_bytes(UART_NUM, (const char *)command, event.size);
                break;
            // Event of HW FIFO overflow detected
            case UART_FIFO_OVF:
                ESP_LOGI(TAG, "hw fifo overflow");
                // If fifo overflow happened, you should consider adding flow control for your application.
                // The ISR has already reset the rx FIFO,
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(UART_NUM);
                xQueueReset(uart1_queue);
                break;
            // Event of UART ring buffer full
            case UART_BUFFER_FULL:
                ESP_LOGI(TAG, "ring buffer full");
                // If buffer full happened, you should consider encreasing your buffer size
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(UART_NUM);
                xQueueReset(uart1_queue);
                break;
            // Event of UART RX break detected
            case UART_BREAK:
                ESP_LOGI(TAG, "uart rx break");
                break;
            // Event of UART parity check error
            case UART_PARITY_ERR:
                ESP_LOGI(TAG, "uart parity error");
                break;
            // Event of UART frame error
            case UART_FRAME_ERR:
                ESP_LOGI(TAG, "uart frame error");
                break;
            // UART_PATTERN_DET
            case UART_PATTERN_DET:
                uart_get_buffered_data_len(UART_NUM, &buffered_size);
                int pos = uart_pattern_pop_pos(UART_NUM);
                ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
                if (pos == -1)
                {
                    // There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
                    // record the position. We should set a larger queue size.
                    // As an example, we directly flush the rx buffer here.
                    uart_flush_input(UART_NUM);
                    break;
                }

                int command_length = uart_read_bytes(UART_NUM, command, pos, 100 / portTICK_PERIOD_MS);
                int pattern_length = uart_read_bytes(UART_NUM, pattern, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);

                if (command_length == -1 || pattern_length == -1)
                {
                    ESP_LOGI(TAG, "command_length: %i", command_length);
                    ESP_LOGI(TAG, "pattern_length : %i", pattern_length);
                    break;
                }

                pattern[pattern_length] = 0;
                command[command_length] = 0;

                ESP_LOGI(TAG, "read pat : %s", pattern);

                command_handler((char *)command);

                break;
            // Others
            default:
                ESP_LOGI(TAG, "uart event type: %d", event.type);
                break;
            }
        }
    }
    free(command);
    free(pattern);
    command = NULL;
    pattern = NULL;
    vTaskDelete(NULL);
}

void app_main(void)
{
    esp_log_level_set(TAG, ESP_LOG_INFO);

    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = UART_BAUDRATE,
        .data_bits = UART_DATA_BITS,
        .parity = UART_PARITY,
        .stop_bits = UART_STOP_BITS,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // Install UART driver, and get the queue.
    uart_driver_install(UART_NUM, UART_BUF_SIZE * 2, UART_BUF_SIZE * 2, 20, &uart1_queue, 0);
    uart_param_config(UART_NUM, &uart_config);

    // Set UART log level
    esp_log_level_set(TAG, ESP_LOG_INFO);
    // Set UART pins (using UART0 default pins ie no changes.)
    uart_set_pin(UART_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Set uart pattern detect function.
    uart_enable_pattern_det_baud_intr(UART_NUM, '+', PATTERN_CHR_NUM, PATTERN_CHR_TIMEOUT, 0, 0);
    // Reset the pattern queue length to record at most 20 pattern positions.
    uart_pattern_queue_reset(UART_NUM, 20);

    // Create a task to handler UART event from ISR
    xTaskCreate(uart_event_task, "uart_event_task", 1024 * 10, NULL, 12, &uart1_handle);

    while (1)
    {
        // PRINT RAM STATUS
        uint32_t free_heap = xPortGetFreeHeapSize();
        uint32_t largestFreeBlock = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
        uint32_t idleSHWM = uxTaskGetStackHighWaterMark(NULL);
        uint32_t uart1SHWM = uxTaskGetStackHighWaterMark(uart1_handle);

        ESP_LOGW(TAG, "lrgsFreeBlck=%lu || freeHeap=%lu || idleSHWM=%lu || uart1SHWM=%lu ",
                 largestFreeBlock, free_heap, idleSHWM, uart1SHWM);

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
