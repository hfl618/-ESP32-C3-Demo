#include "bsp_board.h"
#include "esp_log.h"

static const char *TAG = "BSP_BOARD";

esp_err_t bsp_board_init(void)
{
    ESP_LOGI(TAG, "Initializing Board Hardware...");
    
    // 这里可以预留总线初始化（如 I2C/SPI 共享总线）
    // 目前项目中的 SPI 初始化被集成在 st7735 驱动内，后续可重构到此
    
    return ESP_OK;
}
