#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "uart_service.h"
#include "wifi_service.h"
#include "gui_engine.h"
#include "version.h"
#include "ble_service.h"

#include "freertos/queue.h"
#include "sys_msg.h"

static const char *TAG = "main";

/* --- 全局邮局实体定义 --- */
QueueHandle_t g_sys_msg_queue = NULL;

void app_main(void) {
    ESP_LOGI(TAG, "System Startup. Version: %s", SOFTWARE_VERSION);

    // 1. 邮局开门 (必须在所有服务启动前)
    g_sys_msg_queue = xQueueCreate(10, sizeof(sys_msg_t));

    bsp_board_init();
    uart_service_init();

    // 初始化服务资源，但不开启无线电
    wifi_service_init();
    ble_service_init();

    gui_engine_start();

    ESP_LOGI(TAG, "All systems standby.");

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        /**
         * 暂时注释心跳日志，减少串口干扰。
         * 如需调试内存泄漏，请取消下方注释。
         */
        // ESP_LOGI(TAG, "Keep-alive - Free heap: %lu bytes", esp_get_free_heap_size());
    }
}
