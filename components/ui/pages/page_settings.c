#include "../ui_entry.h"
#include "sys_msg.h"
#include "esp_log.h"

static void settings_item_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    int id = (intptr_t)lv_event_get_user_data(e);
   
    if(code == LV_EVENT_CLICKED) {
        if(id == 4) {
            // [直接处理] 返回主页，属于 UI 导航
            ui_change_page(UI_PAGE_MAIN);
        } else {
            // [外发处理] 各种设置功能的业务逻辑
            extern QueueHandle_t wifi_service_get_queue(void);
            QueueHandle_t q = wifi_service_get_queue();
            if(q) {
                ui_evt_id_t events[] = {UI_EVT_SET_WIFI_CONFIG, UI_EVT_SET_BT_CONFIG, UI_EVT_SET_BRIGHTNESS, UI_EVT_SET_SYSTEM_INFO};
                sys_msg_t msg = { .source = MSG_SOURCE_UI, .event = events[id] };
                xQueueSend(q, &msg, 0);
            }
        }
    }
}

static void create_menu_item(lv_obj_t * parent, const char * icon, const char * text, int id) {
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_set_width(btn, lv_pct(100));
    lv_obj_set_height(btn, 32);
    lv_obj_set_style_radius(btn, 8, 0);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x1F1F1F), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(btn, 0, 0);
    lv_obj_set_style_pad_all(btn, 0, 0);
    lv_obj_set_style_bg_opa(btn, 0, LV_STATE_FOCUSED);
    lv_obj_set_style_border_width(btn, 2, LV_STATE_FOCUSED);
    lv_obj_set_style_border_color(btn, lv_color_white(), LV_STATE_FOCUSED);

    lv_obj_t * lbl_icon = lv_label_create(btn);
    lv_label_set_text(lbl_icon, icon);
    lv_obj_align(lbl_icon, LV_ALIGN_LEFT_MID, 10, 0);
    
    lv_obj_t * lbl_text = lv_label_create(btn);
    lv_label_set_text(lbl_text, text);
    lv_obj_align(lbl_text, LV_ALIGN_LEFT_MID, 35, 0);

    lv_obj_add_event_cb(btn, settings_item_event_cb, LV_EVENT_CLICKED, (void*)(intptr_t)id);
    lv_group_add_obj(lv_group_get_default(), btn);
}

void page_settings_init(lv_obj_t * parent) {
    lv_obj_t * list_cont = lv_obj_create(parent);
    lv_obj_set_size(list_cont, 160, 62);
    lv_obj_set_style_bg_color(list_cont, lv_color_black(), 0);
    lv_obj_set_style_border_width(list_cont, 0, 0);
    lv_obj_set_style_radius(list_cont, 0, 0);
    lv_obj_set_flex_flow(list_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(list_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_ver(list_cont, 16, 0);
    lv_obj_set_style_pad_hor(list_cont, 5, 0);
    lv_obj_set_style_pad_row(list_cont, 6, 0);
    lv_obj_set_scrollbar_mode(list_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_snap_y(list_cont, LV_SCROLL_SNAP_CENTER);

    const char * items_icon[] = {LV_SYMBOL_WIFI, LV_SYMBOL_BLUETOOTH, LV_SYMBOL_EYE_OPEN, LV_SYMBOL_SETTINGS, LV_SYMBOL_LEFT};
    const char * items_text[] = {"WiFi", "Bluetooth", "Display", "System", "Back"};

    for(int i = 0; i < 5; i++) {
        create_menu_item(list_cont, items_icon[i], items_text[i], i);
    }
}
