/**
 * @file page_wifi_info.c
 * @brief WiFi 详细信息展示页面
 */

#include "../../ui_entry.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief 创建详细信息行
 */
static void create_detail_row(lv_obj_t * parent, const char * label, const char * value, lv_color_t color) {
    lv_obj_t * row = lv_obj_create(parent);
    lv_obj_set_size(row, 150, 24);
    lv_obj_set_style_bg_opa(row, 0, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * lbl_title = lv_label_create(row);
    lv_label_set_text(lbl_title, label);
    lv_obj_set_style_text_color(lbl_title, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_align(lbl_title, LV_ALIGN_LEFT_MID, 5, 0);

    lv_obj_t * lbl_value = lv_label_create(row);
    lv_label_set_text(lbl_value, value);
    lv_obj_set_style_text_color(lbl_value, color, 0);
    lv_obj_set_width(lbl_value, 80); 
    lv_label_set_long_mode(lbl_value, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_align(lbl_value, LV_ALIGN_RIGHT_MID, -5, 0);
}

void page_wifi_info_init(lv_obj_t * parent) {
    ui_state_t * state = ui_get_state();
    wifi_profile_t * p = &state->wifi_db.profiles[state->selected_wifi_idx];
    
    // 判断当前显示的 WiFi 是否已成功连接
    bool is_connected = (strlen(state->connected_ssid) > 0 && 
                        strcmp(p->ssid, state->connected_ssid) == 0);
    
    // 决定文字颜色：已连接为绿色，未连接为白色
    lv_color_t text_color = is_connected ? lv_palette_main(LV_PALETTE_GREEN) : lv_color_white();

    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 5, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cont, 2, 0);

    create_detail_row(cont, "SSID", p->ssid, text_color);
    create_detail_row(cont, "Pass", p->password, text_color);

    /**
     * 注意：双击返回上一页逻辑已由全局托管。
     */
}
