#include "bsp_board.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "BSP_BOARD";

esp_err_t bsp_board_init(void)
{
    ESP_LOGI(TAG, "Initializing Board Hardware...");
    
    gpio_config_t bk_conf = {
        .pin_bit_mask = (1ULL << BSP_LCD_BLK_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&bk_conf);
    
    // 明确设置背光高电平
    gpio_set_level(BSP_LCD_BLK_PIN, 1);
    
    // 给背光驱动电路一点建立电压的时间
    vTaskDelay(pdMS_TO_TICKS(50));
    
    ESP_LOGI(TAG, "Backlight powered on.");
    
    return ESP_OK;
}
