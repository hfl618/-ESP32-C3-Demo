#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "st7735s.h"
#include "st7735s_compat.h"
#include "driver/gpio.h"
#include "ui_entry.h"

// 1. 刷新回调
void my_disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map) {
    uint32_t px_count = lv_area_get_width(area) * lv_area_get_height(area);
    lv_draw_sw_rgb565_swap(px_map, px_count);
    uint16_t x1 = area->x1 + XSTART, x2 = area->x2 + XSTART;
    uint16_t y1 = area->y1 + YSTART, y2 = area->y2 + YSTART;
    uint8_t cas[] = {0x2A, x1>>8, x1&0xFF, x2>>8, x2&0xFF};
    uint8_t ras[] = {0x2B, y1>>8, y1&0xFF, y2>>8, y2&0xFF};
    uint8_t ram[] = {0x2C};
    SPI_Transmit(sizeof(cas), cas); SPI_Transmit(sizeof(ras), ras); SPI_TransmitCmd(1, ram);
    SPI_TransmitData(px_count * 2, px_map);
    lv_display_flush_ready(disp);
}

// 2. 编码器结合：使用你之前验证成功的“边沿触发”逻辑
void my_encoder_read(lv_indev_t * indev, lv_indev_data_t * data) {
    static int last_a = 1;
    int a = gpio_get_level(0);
    int b = gpio_get_level(1);
    data->enc_diff = 0;
    if (a != last_a && a == 0) {
        // 关键逻辑：检测下降沿并判断方向
        data->enc_diff = (b == 0) ? 1 : -1;
    }
    last_a = a;
    data->state = (gpio_get_level(5) == 0) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

void gui_task(void *pvParameters) {
    ST7735S_Init();
    // 强制初始化 GPIO
    gpio_config_t io_conf = { 
        .pin_bit_mask = (1ULL<<0)|(1ULL<<1)|(1ULL<<5), 
        .mode = GPIO_MODE_INPUT, .pull_up_en = 1 
    };
    gpio_config(&io_conf);
    
    lv_init();

    lv_display_t * disp = lv_display_create(160, 80);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(disp, my_disp_flush);
    static uint8_t buf1[160 * 20 * 2] __attribute__((aligned(4)));
    lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(indev, my_encoder_read);
    
    lv_group_set_default(lv_group_create());
    lv_indev_set_group(indev, lv_group_get_default());

    // 启动你的 UI
    ui_init();

    while(1) {
        lv_tick_inc(10); // 确保心跳稳定
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void) {
    xTaskCreatePinnedToCore(gui_task, "gui", 8192, NULL, 5, NULL, 0);
    
    // 保持 app_main 运行，防止 USB 串口超时断开
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
