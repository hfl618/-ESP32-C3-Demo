/**
 * @file ui_status_bar.c
 * @brief 顶部状态栏组件实现
 */

#include "../ui_entry.h"

static lv_obj_t * wifi_icon;
static lv_obj_t * bt_icon;
static lv_obj_t * battery_bar;
static lv_obj_t * wifi_warn;
static lv_obj_t * bt_warn;

static lv_obj_t * create_status_icon(lv_obj_t * parent, const char * symbol, lv_obj_t ** warn_out) {
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, 15, 18);
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, symbol);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
    lv_obj_center(label);

    lv_obj_t * warn = lv_label_create(cont);
    lv_label_set_text(warn, "!");
    lv_obj_set_style_text_font(warn, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(warn, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_align(warn, LV_ALIGN_CENTER, 6, -4);
    lv_obj_add_flag(warn, LV_OBJ_FLAG_HIDDEN);

    *warn_out = warn;
    return label;
}

void ui_status_bar_create(lv_obj_t * parent) {
    lv_obj_t * header = lv_obj_create(parent);
    lv_obj_set_size(header, 160, 18);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x222222), 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_pad_all(header, 0, 0);

    lv_obj_t * time_lab = lv_label_create(header);
    lv_label_set_text(time_lab, "12:45");
    lv_obj_set_style_text_font(time_lab, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(time_lab, lv_color_white(), 0);
    lv_obj_align(time_lab, LV_ALIGN_LEFT_MID, 5, 0);

    lv_obj_t * batt_cont = lv_obj_create(header);
    lv_obj_set_size(batt_cont, 26, 12);
    lv_obj_align(batt_cont, LV_ALIGN_RIGHT_MID, -2, 0);
    lv_obj_set_style_bg_opa(batt_cont, 0, 0);
    lv_obj_set_style_border_width(batt_cont, 0, 0);
    lv_obj_set_style_pad_all(batt_cont, 0, 0);

    lv_obj_t * body = lv_obj_create(batt_cont);
    lv_obj_set_size(body, 22, 11);
    lv_obj_align(body, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_radius(body, 2, 0);
    lv_obj_set_style_bg_opa(body, 0, 0);
    lv_obj_set_style_border_width(body, 1, 0);
    lv_obj_set_style_border_color(body, lv_color_white(), 0);
    lv_obj_set_style_pad_all(body, 0, 0); 
    lv_obj_clear_flag(body, LV_OBJ_FLAG_SCROLLABLE);

    battery_bar = lv_bar_create(body);
    lv_obj_set_size(battery_bar, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_radius(battery_bar, 0, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(battery_bar, 0, 0);
    lv_obj_set_style_bg_color(battery_bar, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
    lv_bar_set_value(battery_bar, 100, LV_ANIM_OFF);

    lv_obj_t * tip = lv_obj_create(batt_cont);
    lv_obj_set_size(tip, 3, 5);
    lv_obj_align_to(tip, body, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_color(tip, lv_color_white(), 0);
    lv_obj_set_style_border_width(tip, 0, 0);

    // 优化：图标区域放在电池左侧
    lv_obj_t * icon_area = lv_obj_create(header);
    lv_obj_set_size(icon_area, 40, 18);
    lv_obj_align_to(icon_area, batt_cont, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_set_style_bg_opa(icon_area, 0, 0);
    lv_obj_set_style_border_width(icon_area, 0, 0);
    lv_obj_set_style_pad_all(icon_area, 0, 0);
    lv_obj_set_flex_flow(icon_area, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(icon_area, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(icon_area, 2, 0);

    bt_icon = create_status_icon(icon_area, LV_SYMBOL_BLUETOOTH, &bt_warn);
    wifi_icon = create_status_icon(icon_area, LV_SYMBOL_WIFI, &wifi_warn);
}

void ui_battery_set_level(int level) {
    if(!battery_bar) return;
    int display_level = (level > 0 && level < 15) ? 15 : level;
    lv_bar_set_value(battery_bar, display_level, LV_ANIM_ON);
    if(level <= 20) lv_obj_set_style_bg_color(battery_bar, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);
    else lv_obj_set_style_bg_color(battery_bar, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
}

void ui_status_bar_set_wifi_conn(bool connected) {
    if(!wifi_icon) return;
    if(connected) {
        lv_obj_set_style_text_color(wifi_icon, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_add_flag(wifi_warn, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_set_style_text_color(wifi_icon, lv_palette_main(LV_PALETTE_RED), 0);
        lv_obj_remove_flag(wifi_warn, LV_OBJ_FLAG_HIDDEN);
    }
}

void ui_status_bar_set_bt_conn(bool connected) {
    if(!bt_icon) return;
    if(connected) {
        lv_obj_set_style_text_color(bt_icon, lv_palette_main(LV_PALETTE_CYAN), 0);
        lv_obj_add_flag(bt_warn, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_set_style_text_color(bt_icon, lv_palette_main(LV_PALETTE_RED), 0);
        lv_obj_remove_flag(bt_warn, LV_OBJ_FLAG_HIDDEN);
    }
}
