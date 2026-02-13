#include "uart_service.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "UART_SVC";

esp_err_t uart_service_init(void)
{
    // 不再初始化 UART0 驱动，直接复用系统的 Console
    ESP_LOGI(TAG, "Service initialized (using Console UART)");
    return ESP_OK;
}

void uart_send_string(const char* data)
{
    // 使用 ESP_LOGI 代替 uart_write_bytes，确保不会导致驱动错误
    ESP_LOGI("UART_DATA", "%s", data);
}
