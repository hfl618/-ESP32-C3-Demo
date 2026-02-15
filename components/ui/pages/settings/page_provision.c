/**
 * @file page_provision.c
 * @brief 蓝牙配网管理页面
 * 
 * 职责：展示蓝牙开关，引导用户使用手机 APP 配网。
 */

#include "../../ui_entry.h"

/**
 * @brief 蓝牙开关行点击回调
 */
static void bt_switch_event_cb(lv_event_t * e) {
    lv_obj_t * cont = lv_event_get_target(e);
    lv_obj_t * sw = lv_obj_get_child(cont, 1); 
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        if(lv_obj_has_state(sw, LV_STATE_CHECKED)) lv_obj_remove_state(sw, LV_STATE_CHECKED);
        else lv_obj_add_state(sw, LV_STATE_CHECKED);
        
        bool en = lv_obj_has_state(sw, LV_STATE_CHECKED);
        ui_get_state()->bt_en = en; 
        
        // 模块化投递信号：开启/关闭蓝牙
        ui_post_event(UI_EVT_SET_BT_CONFIG, (intptr_t)en);
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

void page_provision_init(lv_obj_t * parent) {
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE); // 禁止页面滚动
    
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_top(cont, 8, 0);
    lv_obj_set_style_pad_row(cont, 6, 0);

    // 蓝牙开关行容器
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
    lv_label_set_text(lbl, "Bluetooth");
    lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl, ui_get_app_font(), 0);

    lv_obj_t * sw = lv_switch_create(sw_item);
    lv_obj_set_size(sw, 30, 16);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x444444), LV_PART_MAIN);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x34C759), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_remove_flag(sw, LV_OBJ_FLAG_CLICKABLE);

    if(ui_get_state()->bt_en) lv_obj_add_state(sw, LV_STATE_CHECKED);
    lv_obj_add_event_cb(sw_item, bt_switch_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(lv_group_get_default(), sw_item);

    // 引导信息
    lv_obj_t * info_lbl = lv_label_create(cont);
    lv_obj_set_width(info_lbl, lv_pct(90));
    lv_label_set_text(info_lbl, "Use APP to provision via BLE.");
    lv_obj_set_style_text_color(info_lbl, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_style_text_font(info_lbl, ui_get_app_font(), 0);
    lv_label_set_long_mode(info_lbl, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(info_lbl, LV_TEXT_ALIGN_CENTER, 0);
}
