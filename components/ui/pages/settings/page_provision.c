#include "../../ui_entry.h"

static void prov_page_event_cb(lv_event_t * e) {
    if(lv_event_get_code(e) == LV_EVENT_DOUBLE_CLICKED) ui_change_page(UI_PAGE_SETTINGS);
}

static void bt_conn_timer_cb(lv_timer_t * t) { ui_status_bar_set_bt_conn(true); }

static void bt_switch_event_cb(lv_event_t * e) {
    lv_obj_t * sw = lv_event_get_target(e);
    if(lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        bool en = lv_obj_has_state(sw, LV_STATE_CHECKED);
        ui_get_state()->bt_en = en; 
        ui_port_set_bt_power(en);
        if(en) {
            ui_status_bar_set_bt_connecting();
            lv_timer_t * t = lv_timer_create(bt_conn_timer_cb, 2000, NULL);
            lv_timer_set_repeat_count(t, 1);
        } else {
            ui_status_bar_set_bt_conn(false);
        }
    }
}

void page_provision_init(lv_obj_t * parent) {
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_top(cont, 10, 0);
    lv_obj_set_style_pad_row(cont, 8, 0);

    lv_obj_t * sw_item = lv_obj_create(cont);
    lv_obj_set_size(sw_item, lv_pct(90), 32); // 统一宽度百分比
    lv_obj_set_style_bg_color(sw_item, lv_color_hex(0x1A1A1A), 0);
    lv_obj_set_style_border_width(sw_item, 0, 0);
    lv_obj_set_style_radius(sw_item, 8, 0);
    lv_obj_set_flex_flow(sw_item, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(sw_item, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(sw_item, 10, 0); 
    lv_obj_add_flag(sw_item, LV_OBJ_FLAG_EVENT_BUBBLE); 

    lv_obj_t * lbl = lv_label_create(sw_item);
    lv_label_set_text(lbl, "BT");
    lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl, ui_get_app_font(), 0);

    lv_obj_t * sw = lv_switch_create(sw_item);
    lv_obj_set_size(sw, 30, 16);
    
    // --- 统一开关样式 ---
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x444444), LV_PART_MAIN);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0x34C759), LV_PART_INDICATOR | LV_STATE_CHECKED);

    lv_obj_add_flag(sw, LV_OBJ_FLAG_EVENT_BUBBLE); 
    if(ui_get_state()->bt_en) lv_obj_add_state(sw, LV_STATE_CHECKED);
    lv_obj_add_event_cb(sw, bt_switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_group_add_obj(lv_group_get_default(), sw);

    lv_obj_t * info_lbl = lv_label_create(cont);
    lv_obj_set_width(info_lbl, lv_pct(90));
    lv_label_set_text(info_lbl, "Use our mobile app to scan and provision via BLE.");
    lv_obj_set_style_text_color(info_lbl, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_style_text_font(info_lbl, &lv_font_montserrat_10, 0);
    lv_label_set_long_mode(info_lbl, LV_LABEL_LONG_SCROLL_CIRCULAR);

    lv_obj_add_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(cont, prov_page_event_cb, LV_EVENT_DOUBLE_CLICKED, NULL);
}
