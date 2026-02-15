/**
 * @file page_wifi.c
 * @brief WiFi 管理页面：实现 100% 纯净的样式驱动
 * 
 * 职责：仅读取 global_ui_state 进行渲染，不包含任何硬件头文件。
 * 逻辑：基于状态镜像实现连接高亮（绿框）。
 */

#include "../../ui_entry.h"
#include <string.h>

/**
 * @brief 页面全局回调
 */
static void wifi_page_event_cb(lv_event_t * e) {
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        ui_change_page(UI_PAGE_MAIN);
    }
}

/**
 * @brief WiFi 开关容器点击事件
 */
static void wifi_switch_event_cb(lv_event_t * e) {
    lv_obj_t * cont = lv_event_get_target(e);
    lv_obj_t * sw = lv_obj_get_child(cont, 1); 
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        if(lv_obj_has_state(sw, LV_STATE_CHECKED)) lv_obj_remove_state(sw, LV_STATE_CHECKED);
        else lv_obj_add_state(sw, LV_STATE_CHECKED);
        
        bool en = lv_obj_has_state(sw, LV_STATE_CHECKED);
        ui_get_state()->wifi_en = en; 
        
        // 发送开启/关闭信号
        ui_post_event(UI_EVT_MAIN_WIFI_CONNECT, (intptr_t)en);
    }
    else if(code == LV_EVENT_FOCUSED) {
        lv_obj_set_style_bg_color(cont, lv_color_hex(0x333333), 0);
        lv_obj_set_style_border_width(cont, 2, 0);
        lv_obj_set_style_border_color(cont, lv_color_white(), 0);
    }
    else if(code == LV_EVENT_DEFOCUSED) {
        lv_obj_set_style_bg_color(cont, lv_color_hex(0x1A1A1A), 0);
        lv_obj_set_style_border_width(cont, 0, 0);
    }
}

static void wifi_item_event_cb(lv_event_t * e) {
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        intptr_t idx = (intptr_t)lv_event_get_user_data(e);
        ui_get_state()->selected_wifi_idx = (int)idx;
        ui_post_event(UI_EVT_WIFI_CONNECT_BY_IDX, idx);
        ui_change_page(UI_PAGE_WIFI_INFO);
    }
}

/**
 * @brief 创建 WiFi 列表项
 * 
 * @param is_active 是否匹配全局镜像中的“已连接 SSID”
 */
static void create_wifi_item(lv_obj_t * parent, const char * name, bool is_active, int idx) {
    if(!name || name[0] == '\0') return;
    
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_set_size(btn, lv_pct(90), 28);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x1F1F1F), 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x333333), LV_STATE_FOCUSED);
    lv_obj_set_style_radius(btn, 8, 0);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);
    
    // --- 纯净 UI 逻辑：根据全局变量镜像决定是否显示绿框 ---
    if (is_active) {
        lv_obj_set_style_border_width(btn, 2, 0);
        lv_obj_set_style_border_color(btn, lv_palette_main(LV_PALETTE_GREEN), 0);
    } else {
        lv_obj_set_style_border_width(btn, 2, LV_STATE_FOCUSED);
        lv_obj_set_style_border_color(btn, lv_color_white(), LV_STATE_FOCUSED);
    }
    
    lv_obj_t * lbl = lv_label_create(btn);
    lv_label_set_text(lbl, name);
    lv_obj_set_style_text_font(lbl, ui_get_app_font(), 0);
    lv_obj_center(lbl);
    
    lv_obj_add_event_cb(btn, wifi_item_event_cb, LV_EVENT_CLICKED, (void*)(intptr_t)idx);
    lv_group_add_obj(lv_group_get_default(), btn);
}

void page_wifi_init(lv_obj_t * parent) {
    ui_state_t * state = ui_get_state();
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cont, 4, 0);
    lv_obj_set_style_pad_top(cont, 15, 0);
    lv_obj_set_style_pad_bottom(cont, 15, 0);
    lv_obj_set_scroll_snap_y(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    // 1. 开关行
    lv_obj_t * sw_item = lv_obj_create(cont);
    lv_obj_set_size(sw_item, lv_pct(90), 32); 
    lv_obj_set_style_bg_color(sw_item, lv_color_hex(0x1A1A1A), 0);
    lv_obj_set_style_border_width(sw_item, 0, 0);
    lv_obj_set_style_radius(sw_item, 8, 0);
    lv_obj_set_flex_flow(sw_item, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(sw_item, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(sw_item, 12, 0);
    lv_obj_add_flag(sw_item, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(sw_item, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * lbl = lv_label_create(sw_item);
    lv_label_set_text(lbl, "WiFi Power");
    lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl, ui_get_app_font(), 0);

    lv_obj_t * sw = lv_switch_create(sw_item);
    lv_obj_set_size(sw, 30, 16);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x444444), LV_PART_MAIN);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x34C759), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_remove_flag(sw, LV_OBJ_FLAG_CLICKABLE); 

    if(state->wifi_en) lv_obj_add_state(sw, LV_STATE_CHECKED);
    lv_obj_add_event_cb(sw_item, wifi_switch_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(lv_group_get_default(), sw_item);

    // 2. 列表渲染：完全基于状态镜像
    for(int i = 0; i < state->wifi_db.count; i++) {
        // 核心：UI 仅比较内存中的 SSID 字符串，不再询问硬件
        bool is_active = (strlen(state->connected_ssid) > 0 && 
                         strcmp(state->wifi_db.profiles[i].ssid, state->connected_ssid) == 0);
        
        create_wifi_item(cont, state->wifi_db.profiles[i].ssid, is_active, i);
    }

    lv_obj_add_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(cont, wifi_page_event_cb, LV_EVENT_CLICKED, NULL);
}
