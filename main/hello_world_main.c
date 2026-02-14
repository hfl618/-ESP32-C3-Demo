#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bsp_board.h"
#include "uart_service.h"
#include "wifi_service.h"
#include "gui_engine.h"
#include "version.h"

static const char *TAG = "main";

void app_main(void) {
    ESP_LOGI(TAG, "Software Version: %s", SOFTWARE_VERSION);
    ESP_LOGI(TAG, "Hardware Version: %s", HARDWARE_VERSION);
    ESP_LOGI(TAG, "Release Date: %s", VERSION_RELEASE_DATE);

    bsp_board_init();
    uart_service_init();
    wifi_service_init();
    gui_engine_start();

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
