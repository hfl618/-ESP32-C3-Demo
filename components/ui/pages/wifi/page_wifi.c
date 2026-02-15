#include "../../ui_entry.h"

static void wifi_page_event_cb(lv_event_t * e) {
    if(lv_event_get_code(e) == LV_EVENT_DOUBLE_CLICKED) ui_change_page(UI_PAGE_MAIN);
}

static void wifi_switch_event_cb(lv_event_t * e) {
    lv_obj_t * sw = lv_event_get_target(e);
    if(lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        bool en = lv_obj_has_state(sw, LV_STATE_CHECKED);
        ui_get_state()->wifi_en = en; 
        ui_port_set_wifi_power(en);
        if(en) ui_status_bar_set_wifi_connecting();
        else ui_status_bar_set_wifi_conn(false);
    }
}

static void wifi_item_event_cb(lv_event_t * e) {
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) ui_change_page(UI_PAGE_WIFI_INFO);
}

static void create_wifi_item(lv_obj_t * parent, const char * name) {
    if(!name || name[0] == '\0') return;
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_set_size(btn, lv_pct(90), 28); // 统一宽度百分比
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x1F1F1F), 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x333333), LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(btn, 2, LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(btn, lv_color_white(), LV_STATE_FOCUSED);
    lv_obj_set_style_radius(btn, 8, 0);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_t * lbl = lv_label_create(btn);
    lv_label_set_text(lbl, name);
    lv_obj_set_style_text_font(lbl, ui_get_app_font(), 0);
    lv_obj_center(lbl);
    lv_obj_add_event_cb(btn, wifi_item_event_cb, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(lv_group_get_default(), btn);
}

void page_wifi_init(lv_obj_t * parent) {
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cont, 4, 0);
    lv_obj_set_style_pad_top(cont, 20, 0);
    lv_obj_set_style_pad_bottom(cont, 20, 0);
    lv_obj_set_scroll_snap_y(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t * sw_item = lv_obj_create(cont);
    lv_obj_set_size(sw_item, lv_pct(90), 32); // 统一宽度百分比
    lv_obj_set_style_bg_color(sw_item, lv_color_hex(0x1A1A1A), 0);
    lv_obj_set_style_border_width(sw_item, 0, 0);
    lv_obj_set_style_radius(sw_item, 8, 0);
    lv_obj_set_flex_flow(sw_item, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(sw_item, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(sw_item, 12, 0);
    lv_obj_add_flag(sw_item, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t * lbl = lv_label_create(sw_item);
    lv_label_set_text(lbl, "WIFI");
    lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl, ui_get_app_font(), 0);

    lv_obj_t * sw = lv_switch_create(sw_item);
    lv_obj_set_size(sw, 30, 16);
    
    // --- 统一开关样式 ---
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x444444), LV_PART_MAIN);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x34C759), LV_PART_INDICATOR | LV_STATE_CHECKED);

    lv_obj_add_flag(sw, LV_OBJ_FLAG_EVENT_BUBBLE);
    if(ui_get_state()->wifi_en) lv_obj_add_state(sw, LV_STATE_CHECKED);
    lv_obj_add_event_cb(sw, wifi_switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_group_add_obj(lv_group_get_default(), sw);

    const char * mock_data[3] = {"ChinaNet-5G", "MERCURY_2.4G", ""};
    for(int i = 0; i < 3; i++) create_wifi_item(cont, mock_data[i]);

    lv_obj_add_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(cont, wifi_page_event_cb, LV_EVENT_DOUBLE_CLICKED, NULL);
}
