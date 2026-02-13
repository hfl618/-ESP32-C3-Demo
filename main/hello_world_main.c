#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lvgl.h"
#include "st7735s.h"
#include "st7735s_compat.h"
#include "ec11.h"
#include "ui_entry.h"
#include "uart_service.h"
#include "bsp_board.h"
#include "driver/gpio.h"

static const char *TAG = "APP_MAIN";

void my_disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map) {
    uint32_t px_count = lv_area_get_width(area) * lv_area_get_height(area);
    lv_draw_sw_rgb565_swap(px_map, px_count);  
    uint16_t x1 = area->x1 + defXSTART, x2 = area->x2 + defXSTART;
    uint16_t y1 = area->y1 + defYSTART, y2 = area->y2 + defYSTART;
    uint8_t cas[] = {0x2A, x1>>8, x1&0xFF, x2>>8, x2&0xFF};
    uint8_t ras[] = {0x2B, y1>>8, y1&0xFF, y2>>8, y2&0xFF};
    uint8_t ram[] = {0x2C};
    SPI_Transmit(sizeof(cas), cas); SPI_Transmit(sizeof(ras), ras); SPI_TransmitCmd(1, ram);   
    SPI_TransmitData(px_count * 2, px_map);    
    lv_display_flush_ready(disp);
}

void my_Indev_read(lv_indev_t * indev, lv_indev_data_t * data) {
    data->enc_diff = ec11_get_delta();
    if(ec11_is_pressed()) {
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void gui_task(void *pvParameters) {
    ST7735S_Init();
    uart_service_init();
    ec11_init();
    lv_init();
    
    lv_display_t * disp = lv_display_create(160, 80);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(disp, my_disp_flush);
    static uint8_t buf1[160 * 20 * 2];
    lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);    

    lv_group_t * g = lv_group_create();
    lv_group_set_default(g);

    lv_indev_t * indev = lv_indev_create();    
    lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(indev, my_Indev_read);
    lv_indev_set_group(indev, g);

    ui_init();

    int count = 0;
    while(1) {
        lv_timer_handler();
        lv_tick_inc(10); 
        if (count++ % 100 == 0) {
            extern volatile int pcnt_count;
            int sw_level = gpio_get_level(5); // BSP_EC11_SW_PIN
            ESP_LOGI(TAG, "HB:%d | Enc:%d | SW:%d", count/100, pcnt_count, sw_level);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void) {
    bsp_board_init();
    // 增加任务栈到 16KB，防止切换页面时溢出导致重启
    xTaskCreatePinnedToCore(gui_task, "gui", 16384, NULL, 5, NULL, 0);
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
