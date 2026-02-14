#include "gui_engine.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "st7735s.h"
#include "st7735s_compat.h"
#include "ec11.h"
#include "ui_entry.h"
#include "wifi_service.h"
#include "sys_msg.h"
#include "driver/gpio.h"
#include <time.h>

static const char *TAG = "GUI_ENGINE";

static uint32_t get_tick_cb(void) {
    return (uint32_t)(esp_timer_get_time() / 1000);
}

/* --- [核心逻辑] 处理时间并推送给 UI --- */
static void update_system_clock(void) {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    // 如果年份太小，说明还没对过时，显示横线
    if (timeinfo.tm_year < (2020 - 1900)) {
        ui_status_bar_set_time("--:--");
    } else {
        char buf[16];
        sprintf(buf, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
        ui_status_bar_set_time(buf);
    }
}

static void disp_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map) {
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

static void indev_read_cb(lv_indev_t * indev, lv_indev_data_t * data) {
    data->enc_diff = ec11_get_delta();
    data->state = ec11_is_pressed() ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

static void gui_msg_dispatcher(QueueHandle_t queue) {
    sys_msg_t msg;
    while (xQueueReceive(queue, &msg, 0) == pdTRUE) {
        switch (msg.source) {
            case MSG_SOURCE_WIFI:
                if (msg.event == WIFI_EVT_GOT_IP) ui_status_bar_set_wifi_conn(true);
                else if (msg.event == WIFI_EVT_DISCONNECTED) ui_status_bar_set_wifi_conn(false);
                else if (msg.event == WIFI_EVT_TIME_SYNCED) {
                    ESP_LOGI(TAG, "NTP Synced. Updating Clock.");
                    update_system_clock(); // 逻辑处理后推送给视图
                }
                break;
            case MSG_SOURCE_UI:
                if (msg.event == UI_EVT_MAIN_WIFI_CONNECT) wifi_service_connect();
                break;
            default: break;
        }
    }
}

static void gui_task(void *pvParameters) {
    ST7735S_Init();
    ec11_init();
    lv_init();
    setenv("TZ", "CST-8", 1);
    tzset();

    lv_tick_set_cb(get_tick_cb);
    lv_display_t * disp = lv_display_create(160, 80);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(disp, disp_flush_cb);
    static uint8_t buf1[160 * 20 * 2];
    lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);    

    lv_group_t * g = lv_group_create();
    lv_group_set_default(g);
    lv_indev_t * indev = lv_indev_create();    
    lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(indev, indev_read_cb);
    lv_indev_set_group(indev, g);

    ui_init();

    QueueHandle_t sys_queue = wifi_service_get_queue();
    uint32_t last_time_update = 0;

    while(1) {
        gui_msg_dispatcher(sys_queue); 
        lv_timer_handler();            
        
        // 每 30 秒刷一次时钟
        if (lv_tick_get() - last_time_update > 30000) {
            update_system_clock(); 
            last_time_update = lv_tick_get();
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void gui_engine_start(void) {
    xTaskCreatePinnedToCore(gui_task, "gui_task", 16384, NULL, 5, NULL, 0);
}
