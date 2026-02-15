/**
 * @file ui_entry.h
 * @brief UI 系统的全局接口定义
 */

#ifndef UI_ENTRY_H
#define UI_ENTRY_H

#include "lvgl.h"
#include "sys_msg.h" 
#include "wifi_service.h" // 引入 wifi_db_t 定义
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * --- UI 专用发信代理 ---
 * 
 * 优势：
 * 1. 模块化：UI 页面不需要包含 FreeRTOS 或了解队列句柄。
 * 2. 零开销：使用 static inline 编译优化，直接展开为内核调用。
 * 3. 纯净化：保持 UI 文件夹仅作为样式源码。
 */
static inline void ui_post_event(int event_id, intptr_t val) {
    sys_msg_send(MSG_SOURCE_UI, event_id, (void*)val);
}

typedef enum {
    UI_PAGE_MAIN,
    UI_PAGE_SETTINGS,
    UI_PAGE_WIFI,
    UI_PAGE_WIFI_INFO,
    UI_PAGE_VERSION,
    UI_PAGE_PROVISION,
    UI_PAGE_DISPLAY,
    UI_PAGE_BRIGHTNESS,
    UI_PAGE_FONTSIZE,
    UI_PAGE_VOLUME,
    UI_PAGE_LOADING,
    UI_PAGE_ALERT,
} ui_page_t;

typedef enum {
    UI_ALERT_TYPE_INFO,    // 蓝色信息
    UI_ALERT_TYPE_SUCCESS, // 绿色成功
    UI_ALERT_TYPE_ERROR,   // 红色错误
    UI_ALERT_TYPE_AI,      // AI 对话专用 (机器人头像)
} ui_alert_type_t;

/* --- 全局状态管理 --- */
typedef struct {
    bool wifi_en;
    bool bt_en;
    bool status_bar_en;
    int brightness;
    int volume;
    int font_size_level;
    wifi_db_t wifi_db; // 缓存的真实 WiFi 列表数据
    char connected_ssid[32]; // 【镜像】当前已成功连接的热点名字
    char connected_ip[16];   // 【新增】镜像当前连接的 IP
    int selected_wifi_idx;   // 【新增】当前正在查看或尝试连接的 WiFi 索引
} ui_state_t;

ui_state_t * ui_get_state(void);
void ui_refresh_current_page(void);

/* --- 硬件接口 --- */
void ui_port_set_brightness(int value);
void ui_port_set_volume(int value);
void ui_port_set_wifi_power(bool en);
void ui_port_set_bt_power(bool en);

/* --- 通用模块接口 --- */
void ui_show_loading(const char * text, const void * icon_src, uint32_t duration, ui_page_t target_page);
void ui_update_loading_text(const char * text);
void ui_finish_loading(ui_page_t target_page);
void ui_show_alert(const char * title, const char * msg, ui_alert_type_t type, ui_page_t target_page);
void ui_show_alert_with_icon(const char * title, const char * msg, const void * icon_src, ui_page_t target_page);
void ui_update_alert_text(const char * msg);

const lv_font_t * ui_get_app_font(void);
void ui_set_app_font_size(int level);

void ui_init(void);
void ui_change_page(ui_page_t page);
void ui_back_to_prev(void);
ui_page_t ui_get_current_page(void);

void ui_status_bar_create(lv_obj_t * parent);
void ui_status_bar_set_visible(bool visible);
void ui_battery_set_level(int level);
void ui_status_bar_set_wifi_conn(bool connected);
void ui_status_bar_set_wifi_connecting(void);
void ui_status_bar_set_wifi_error(void);
void ui_status_bar_set_bt_conn(bool connected);
void ui_status_bar_set_bt_connecting(void);
void ui_status_bar_set_bt_error(void);
void ui_status_bar_set_time(const char *time_str); 

void page_main_init(lv_obj_t * parent);
void page_settings_init(lv_obj_t * parent);
void page_version_init(lv_obj_t * parent);
void page_provision_init(lv_obj_t * parent);
void page_display_init(lv_obj_t * parent);
void page_brightness_init(lv_obj_t * parent);
void page_fontsize_init(lv_obj_t * parent);
void page_volume_init(lv_obj_t * parent);
void page_wifi_init(lv_obj_t * parent);
void page_wifi_info_init(lv_obj_t * parent);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_ENTRY_H*/
