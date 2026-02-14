#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bsp_board.h"
#include "uart_service.h"
#include "wifi_service.h"
#include "gui_engine.h"

void app_main(void) {
    bsp_board_init();
    uart_service_init();
    wifi_service_init();
    gui_engine_start();

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
