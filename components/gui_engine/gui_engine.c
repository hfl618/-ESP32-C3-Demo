/**
 * @file gui_engine.c
 * @brief GUI 引擎核心：负责信号分发、输入驱动及 UI 逻辑中枢
 */

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
#include "esp_wifi.h"
#include "ble_service.h"
#include "sys_msg.h"
#include "driver/gpio.h"
#include <time.h>

static const char *TAG = "GUI_ENGINE";

static uint32_t get_tick_cb(void) {
    return (uint32_t)(esp_timer_get_time() / 1000);
}

/* --- [内部逻辑] 处理系统时钟更新 --- */
static void update_system_clock(void) {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    if (timeinfo.tm_year < (2020 - 1900)) {
        ui_status_bar_set_time("--:--");
    } else {
        char buf[16];
        sprintf(buf, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
        ui_status_bar_set_time(buf);
    }
}

/* --- [内部逻辑] WiFi 错误显示超时回调 --- */
static void wifi_error_timer_cb(lv_timer_t * t) {
    ui_status_bar_set_wifi_conn(false); 
}

/**
 * @brief 核心信号分发器
 */
static void gui_msg_dispatcher(QueueHandle_t queue) {
    sys_msg_t msg;
    ui_state_t * state = ui_get_state();
    static bool last_bt_st = false; 

    while (xQueueReceive(queue, &msg, 0) == pdTRUE) {
        switch (msg.source) {
            case MSG_SOURCE_WIFI:
                if (msg.event == WIFI_EVT_GOT_IP) {
                    ui_status_bar_set_wifi_conn(true);
                    
                    // --- 核心同步逻辑：将硬件层状态同步到 UI 全局镜像 ---
                    wifi_ap_record_t ap_info;
                    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
                        strncpy(state->connected_ssid, (char*)ap_info.ssid, 32);
                    }

                    // 获取并同步 IP 地址
                    esp_netif_ip_info_t ip_info;
                    esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
                    if (netif && esp_netif_get_ip_info(netif, &ip_info) == ESP_OK) {
                        esp_ip4addr_ntoa(&ip_info.ip, state->connected_ip, 16);
                    }

                    // --- [新增] 自动排序逻辑：将当前成功的 WiFi 置顶 ---
                    if (state->selected_wifi_idx > 0 && state->selected_wifi_idx < state->wifi_db.count) {
                        wifi_profile_t successful_p = state->wifi_db.profiles[state->selected_wifi_idx];
                        // 将第 0 位和当前位互换（或整体平移，互换最简单且有效）
                        state->wifi_db.profiles[state->selected_wifi_idx] = state->wifi_db.profiles[0];
                        state->wifi_db.profiles[0] = successful_p;
                        state->selected_wifi_idx = 0;
                        // 保存到 NVS，下次开机自动秒连此热点
                        wifi_service_save_db(&state->wifi_db);
                        ESP_LOGI(TAG, "WiFi DB Reordered: %s is now preferred", successful_p.ssid);
                    }

                    ui_page_t cur = ui_get_current_page();
                    if (cur == UI_PAGE_WIFI || cur == UI_PAGE_WIFI_INFO) {
                        ui_refresh_current_page();
                    }
                }
                else if (msg.event == WIFI_EVT_DISCONNECTED) {
                    // 清空镜像状态
                    memset(state->connected_ssid, 0, sizeof(state->connected_ssid));
                    memset(state->connected_ip, 0, sizeof(state->connected_ip));

                    // 连接失败或断开时，自动关闭开关
                    state->wifi_en = false;
                    wifi_service_off();

                    ui_status_bar_set_wifi_error();
                    lv_timer_t * t = lv_timer_create(wifi_error_timer_cb, 5000, NULL);
                    lv_timer_set_repeat_count(t, 1);
                    
                    ui_page_t cur = ui_get_current_page();
                    if (cur == UI_PAGE_WIFI || cur == UI_PAGE_WIFI_INFO) {
                        ui_refresh_current_page();
                    }
                }
                else if (msg.event == WIFI_EVT_TIME_SYNCED) {
                    update_system_clock();
                }
                break;

            case MSG_SOURCE_UI:
                if (msg.event == UI_EVT_MAIN_WIFI_CONNECT) {
                    bool en = (bool)(intptr_t)msg.data;
                    state->wifi_en = en;
                    if (en) {
                        state->bt_en = false;
                        last_bt_st = false;
                        ui_status_bar_set_bt_conn(false);
                        ble_service_off();
                        ui_status_bar_set_wifi_connecting();
                        wifi_service_on();
                        ui_refresh_current_page(); // 开启时也强制刷新一次，确保开关状态稳固
                    } else {
                        wifi_service_off();
                        ui_status_bar_set_wifi_conn(false);
                        // --- [新增] 手动关闭时清理状态并刷新页面 ---
                        memset(state->connected_ssid, 0, sizeof(state->connected_ssid));
                        memset(state->connected_ip, 0, sizeof(state->connected_ip));
                        ui_refresh_current_page();
                    }
                }
                else if (msg.event == UI_EVT_SET_BT_CONFIG) {
                    bool en = (bool)(intptr_t)msg.data;
                    if (en == last_bt_st) break; 
                    last_bt_st = en;
                    state->bt_en = en;

                    if (en) {
                        state->wifi_en = false; 
                        ui_status_bar_set_wifi_conn(false);
                        wifi_service_off();
                        vTaskDelay(pdMS_TO_TICKS(150)); // 略微增加延时确保射频彻底切换
                        ui_status_bar_set_bt_connecting();
                        ble_service_on();
                    } else {
                        ble_service_off();
                        ui_status_bar_set_bt_conn(false);
                    }
                }
                else if (msg.event == UI_EVT_SET_BRIGHTNESS) {
                    int b = (int)(intptr_t)msg.data;
                    ESP_LOGI(TAG, "Hardware API: Backlight -> %d%%", b);
                }
                else if (msg.event == UI_EVT_SET_VOLUME) {
                    int v = (int)(intptr_t)msg.data;
                    ESP_LOGI(TAG, "Hardware API: Volume -> %d%%", v);
                }
                else if (msg.event == UI_EVT_SET_SYSTEM_INFO) {
                    bool en = (bool)(intptr_t)msg.data;
                    ui_status_bar_set_visible(en);
                }
                else if (msg.event == UI_EVT_SET_BACK_TO_PREV) {
                    ui_back_to_prev();
                }
                else if (msg.event == UI_EVT_WIFI_CONNECT_BY_IDX) {
                    int idx = (int)(intptr_t)msg.data;
                    state->wifi_en = true;
                    state->selected_wifi_idx = idx;
                    ui_status_bar_set_wifi_connecting();
                    wifi_service_connect_to(state->wifi_db.profiles[idx].ssid, 
                                           state->wifi_db.profiles[idx].password);
                }
                break;
            default: break;
        }
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
    static uint32_t last_phys_press_time = 0;
    static uint32_t last_phys_release_time = 0;
    static bool last_phys_state = false;
    static int phys_click_count = 0;
    static bool lv_is_pressed = false;
    static uint32_t lv_press_start_time = 0;

    data->enc_diff = ec11_get_delta();
    bool phys_pressed = ec11_is_pressed();
    uint32_t now = get_tick_cb();

    if (lv_is_pressed && phys_click_count == 0 && !phys_pressed) {
        if (now - lv_press_start_time > 30) lv_is_pressed = false;
    }

    if (phys_pressed && !last_phys_state) { 
        phys_click_count++;
        last_phys_press_time = now;
        if (phys_click_count == 2) { 
            if (now - last_phys_release_time < 400) { 
                if (ui_get_current_page() != UI_PAGE_MAIN) {
                    sys_msg_send(MSG_SOURCE_UI, UI_EVT_SET_BACK_TO_PREV, NULL);
                }
                phys_click_count = 0;
                lv_is_pressed = false; 
            } else {
                phys_click_count = 1; 
            }
        }
    } else if (!phys_pressed && last_phys_state) { 
        last_phys_release_time = now;
        if (lv_is_pressed) lv_is_pressed = false;
    }
    last_phys_state = phys_pressed;

    if (phys_click_count == 1) {
        if (!phys_pressed) { 
            if (now - last_phys_release_time > 150) {
                lv_is_pressed = true;
                lv_press_start_time = now;
                phys_click_count = 0; 
            }
        } else if (now - last_phys_press_time > 500) { 
            lv_is_pressed = true;
            lv_press_start_time = now;
            phys_click_count = 0;
        }
    }
    data->state = lv_is_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
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
    update_system_clock();

    wifi_service_load_db(&(ui_get_state()->wifi_db));

    // --- [新增] 开机自动连接 ---
    if (ui_get_state()->wifi_db.count > 0) {
        ui_get_state()->wifi_en = true;
        ui_status_bar_set_wifi_connecting();
        wifi_service_on();
    }

    QueueHandle_t sys_queue = wifi_service_get_queue();
    uint32_t last_time_update = 0;

    while(1) {
        gui_msg_dispatcher(sys_queue); 
        lv_timer_handler();            
        uint32_t now = lv_tick_get();
        if (now - last_time_update > 30000) {
            update_system_clock(); 
            last_time_update = now;
        }
        static uint32_t last_heartbeat = 0;
        if (now - last_heartbeat > 60000) {
            ESP_LOGI(TAG, "GUI Task Alive");
            last_heartbeat = now;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void gui_engine_start(void) {
    xTaskCreatePinnedToCore(gui_task, "gui_task", 16384, NULL, 5, NULL, 0);
}
