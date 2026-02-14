/**
 * @file ui_entry.h
 * @brief UI 系统的全局接口定义
 */

#ifndef UI_ENTRY_H
#define UI_ENTRY_H

#include "lvgl.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UI_PAGE_MAIN,
    UI_PAGE_SETTINGS,
} ui_page_t;

void ui_init(void);
void ui_change_page(ui_page_t page);

void ui_status_bar_create(lv_obj_t * parent);
void ui_battery_set_level(int level);
void ui_status_bar_set_wifi_conn(bool connected);
void ui_status_bar_set_bt_conn(bool connected);

/* --- 纯净的设置接口：只接收字符串，不处理逻辑 --- */
void ui_status_bar_set_time(const char *time_str); 

void page_main_init(lv_obj_t * parent);
void page_settings_init(lv_obj_t * parent);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_ENTRY_H*/
