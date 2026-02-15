#include "../../ui_entry.h"
#include <stdio.h>

static void settings_page_event_cb(lv_event_t * e) {
    if(lv_event_get_code(e) == LV_EVENT_DOUBLE_CLICKED) ui_change_page(UI_PAGE_MAIN);
}

static void settings_item_event_cb(lv_event_t * e) {
    int id = (intptr_t)lv_event_get_user_data(e);
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        // --- 修正后的 ID 映射 ---
        if(id == 0) ui_change_page(UI_PAGE_PROVISION);
        else if(id == 1) ui_change_page(UI_PAGE_VOLUME);
        else if(id == 3) ui_change_page(UI_PAGE_DISPLAY);
        else if(id == 4) ui_change_page(UI_PAGE_VERSION);
    }
}

static void create_menu_item(lv_obj_t * parent, const char * icon, const char * text, int id) {
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_set_width(btn, lv_pct(90)); 
    lv_obj_set_height(btn, 32);         
    lv_obj_set_style_radius(btn, 8, 0); 
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x1F1F1F), 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x333333), LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(btn, 2, LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(btn, lv_color_white(), LV_STATE_FOCUSED);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t * lbl_i = lv_label_create(btn);
    lv_label_set_text(lbl_i, icon);
    lv_obj_align(lbl_i, LV_ALIGN_LEFT_MID, 10, 0);
    
    lv_obj_t * lbl_t = lv_label_create(btn);
    lv_label_set_text(lbl_t, text);
    lv_obj_set_style_text_font(lbl_t, ui_get_app_font(), 0);
    lv_obj_align(lbl_t, LV_ALIGN_LEFT_MID, 35, 0);

    lv_obj_add_event_cb(btn, settings_item_event_cb, LV_EVENT_CLICKED, (void*)(intptr_t)id);
    lv_group_add_obj(lv_group_get_default(), btn);
}

void page_settings_init(lv_obj_t * parent) {
    lv_obj_t * list = lv_obj_create(parent);
    lv_obj_set_size(list, lv_pct(100), lv_pct(100)); 
    lv_obj_set_style_bg_opa(list, 0, 0);
    lv_obj_set_style_border_width(list, 0, 0);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(list, 6, 0);
    lv_obj_set_style_pad_ver(list, 15, 0);
    lv_obj_set_scroll_snap_y(list, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);

    const char * icons[] = {LV_SYMBOL_DOWNLOAD, LV_SYMBOL_VOLUME_MAX, LV_SYMBOL_BLUETOOTH, LV_SYMBOL_EYE_OPEN, LV_SYMBOL_SETTINGS};
    const char * texts[] = {"Provision", "Volume", "Bluetooth", "Display", "Version"};

    for(int i = 0; i < 5; i++) create_menu_item(list, icons[i], texts[i], i);

    lv_obj_add_flag(list, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(list, settings_page_event_cb, LV_EVENT_DOUBLE_CLICKED, NULL);
}
