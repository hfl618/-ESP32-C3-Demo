#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "uart_service.h"
#include "wifi_service.h"
#include "gui_engine.h"
#include "version.h"
#include "ble_service.h"

static const char *TAG = "main";

#define ENABLE_BLE_PROVISIONING 0 // 暂时设为 0 关闭，改为 1 即可恢复蓝牙

void app_main(void) {
    ESP_LOGI(TAG, "System Startup. Version: %s", SOFTWARE_VERSION);

    bsp_board_init();
    uart_service_init();

    // 初始化服务资源，但不开启无线电
    wifi_service_init();
    ble_service_init();

    gui_engine_start();

    ESP_LOGI(TAG, "All systems standby.");

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        ESP_LOGI(TAG, "Keep-alive - Free heap: %lu bytes", esp_get_free_heap_size());
    }
}
