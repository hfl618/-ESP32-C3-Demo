/**
 * @file page_display.c
 * @brief 显示设置页面
 * 
 * 职责：状态栏开关、亮度/字体跳转。
 */

#include "../../ui_entry.h"

/**
 * @brief 显示设置项事件处理
 */
static void display_item_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    int id = (intptr_t)lv_event_get_user_data(e);
    
    if(code == LV_EVENT_CLICKED) {
        if(id == 0) ui_change_page(UI_PAGE_BRIGHTNESS);
        else if(id == 1) ui_change_page(UI_PAGE_FONTSIZE);
        else if(id == -1) ui_change_page(UI_PAGE_SETTINGS); // 背景返回
    } else if(code == LV_EVENT_VALUE_CHANGED && id == 2) {
        // 状态栏开关逻辑
        lv_obj_t * sw = lv_event_get_target(e);
        bool en = lv_obj_has_state(sw, LV_STATE_CHECKED);
        ui_get_state()->status_bar_en = en; 
        
        // 模块化投递信号：通过总线请求显隐状态栏
        ui_post_event(UI_EVT_SET_SYSTEM_INFO, (intptr_t)en);
    }
}

/**
 * @brief 创建开关列表项
 */
static void create_switch_item(lv_obj_t * parent, const char * text, int id) {
    lv_obj_t * row = lv_obj_create(parent);
    lv_obj_set_size(row, lv_pct(90), 32);
    lv_obj_set_style_bg_color(row, lv_color_hex(0x1F1F1F), 0);
    lv_obj_set_style_bg_color(row, lv_color_hex(0x333333), LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(row, 2, LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(row, lv_color_white(), LV_STATE_FOCUSED);
    lv_obj_set_style_radius(row, 8, 0);
    lv_obj_add_flag(row, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t * lbl = lv_label_create(row);
    lv_label_set_text(lbl, text);
    lv_obj_set_style_text_font(lbl, ui_get_app_font(), 0);
    lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
    lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 10, 0);

    lv_obj_t * sw = lv_switch_create(row);
    lv_obj_set_size(sw, 30, 16);
    lv_obj_align(sw, LV_ALIGN_RIGHT_MID, -5, 0);
    
    // 开关样式
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x444444), LV_PART_MAIN);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x34C759), LV_PART_INDICATOR | LV_STATE_CHECKED);
    
    if(ui_get_state()->status_bar_en) lv_obj_add_state(sw, LV_STATE_CHECKED);
    
    lv_obj_add_event_cb(sw, display_item_event_cb, LV_EVENT_VALUE_CHANGED, (void*)(intptr_t)id);
    lv_group_add_obj(lv_group_get_default(), row);
}

/**
 * @brief 创建普通按钮列表项
 */
static void create_display_menu_item(lv_obj_t * parent, const char * text, int id) {
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_set_size(btn, lv_pct(90), 32);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x1F1F1F), 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x333333), LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(btn, 2, LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(btn, lv_color_white(), LV_STATE_FOCUSED);
    lv_obj_set_style_radius(btn, 8, 0);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);
    
    lv_obj_t * lbl = lv_label_create(btn);
    lv_label_set_text(lbl, text);
    lv_obj_set_style_text_font(lbl, ui_get_app_font(), 0);
    lv_obj_center(lbl);
    
    lv_obj_add_event_cb(btn, display_item_event_cb, LV_EVENT_CLICKED, (void*)(intptr_t)id);
    lv_group_add_obj(lv_group_get_default(), btn);
}

void page_display_init(lv_obj_t * parent) {
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cont, 6, 0);
    lv_obj_set_style_pad_top(cont, 15, 0);
    lv_obj_set_style_pad_bottom(cont, 15, 0);
    lv_obj_set_scroll_snap_y(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    create_switch_item(cont, "Status Bar", 2);
    create_display_menu_item(cont, "Brightness", 0);
    create_display_menu_item(cont, "Font Size", 1);

    lv_obj_add_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(cont, display_item_event_cb, LV_EVENT_CLICKED, (void*)(intptr_t)-1);
}
