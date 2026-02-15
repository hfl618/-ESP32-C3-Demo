#include "../../ui_entry.h"
#include <stdio.h>

static void wifi_info_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_DOUBLE_CLICKED) {
        printf("WiFi Info: Double Clicked! Returning to WiFi List.\n");
        ui_change_page(UI_PAGE_WIFI);
    }
}

static void create_detail_row(lv_obj_t * parent, const char * label, const char * value) {
    lv_obj_t * row = lv_obj_create(parent);
    lv_obj_set_size(row, 150, 24);
    lv_obj_set_style_bg_opa(row, 0, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
    
    // 开启冒泡
    lv_obj_add_flag(row, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t * lbl_title = lv_label_create(row);
    lv_label_set_text(lbl_title, label);
    lv_obj_set_style_text_color(lbl_title, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_align(lbl_title, LV_ALIGN_LEFT_MID, 5, 0);
    lv_obj_add_flag(lbl_title, LV_OBJ_FLAG_EVENT_BUBBLE); // 开启冒泡

    lv_obj_t * lbl_value = lv_label_create(row);
    lv_label_set_text(lbl_value, value);
    lv_obj_set_style_text_color(lbl_value, lv_color_white(), 0);
    lv_obj_set_width(lbl_value, 80); 
    lv_label_set_long_mode(lbl_value, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_align(lbl_value, LV_ALIGN_RIGHT_MID, -5, 0);
    lv_obj_add_flag(lbl_value, LV_OBJ_FLAG_EVENT_BUBBLE); // 开启冒泡
}

void page_wifi_info_init(lv_obj_t * parent) {
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 5, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cont, 2, 0);

    create_detail_row(cont, "SSID", "TP-LINK_Test_Router");
    create_detail_row(cont, "Pass", "admin88888888");

    lv_obj_add_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(cont, wifi_info_event_cb, LV_EVENT_DOUBLE_CLICKED, NULL);
}
