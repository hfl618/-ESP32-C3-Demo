#include "../ui_entry.h"
#include "esp_log.h"
#include "sys_msg.h"

LV_IMAGE_DECLARE(settings);
LV_IMAGE_DECLARE(wifi);
LV_IMAGE_DECLARE(music);
LV_IMAGE_DECLARE(message_ai);

static const struct {
    const lv_image_dsc_t * img_src;
} my_menu[] = {
    {&settings}, {&wifi}, {&music}, {&message_ai}, {&settings} 
};

static void item_event_cb(lv_event_t * e) {
    lv_obj_t * obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    int index = (intptr_t)lv_event_get_user_data(e);

    if(code == LV_EVENT_FOCUSED) {
        ESP_LOGI("UI_MAIN", "Focused Item: %d", index);
        lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x333333), 0); 
        lv_obj_set_style_outline_width(obj, 2, 0);
        lv_obj_set_style_outline_color(obj, lv_color_white(), 0);
        lv_obj_set_style_outline_pad(obj, 2, 0);
        lv_obj_set_style_opa(obj, 255, 0);
        lv_obj_scroll_to_view(obj, LV_ANIM_ON);
    }
    else if(code == LV_EVENT_DEFOCUSED) {
        lv_obj_set_style_bg_opa(obj, 0, 0);
        lv_obj_set_style_outline_width(obj, 0, 0);
        lv_obj_set_style_opa(obj, 150, 0); 
    }
    else if(code == LV_EVENT_PRESSED) {
        lv_obj_set_style_transform_scale(obj, 240, 0);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x111111), 0);
    }
    else if(code == LV_EVENT_RELEASED || code == LV_EVENT_CANCEL) {
        lv_obj_set_style_transform_scale(obj, 256, 0);
        if(lv_obj_has_state(obj, LV_STATE_FOCUSED)) {
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x333333), 0);
        }
    }
    else if(code == LV_EVENT_CLICKED) {
        if(index == 0) {
            // [直接处理] 导航属于 UI 内部逻辑，保留在 UI 层
            ui_change_page(UI_PAGE_SETTINGS);
        }
        else if(index == 1) {
            // [外发处理] 硬件业务逻辑，发送给 Engine 处理
            extern QueueHandle_t wifi_service_get_queue(void);
            QueueHandle_t q = wifi_service_get_queue();
            if (q) {
                sys_msg_t msg = { .source = MSG_SOURCE_UI, .event = UI_EVT_MAIN_WIFI_CONNECT };
                xQueueSend(q, &msg, 0);
            }
        }
    }
}

void page_main_init(lv_obj_t * parent) {
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, 160, 62);
    lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_left(cont, 60, 0);
    lv_obj_set_style_pad_right(cont, 60, 0);
    lv_obj_set_style_pad_column(cont, 25, 0);
    lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    for(uint32_t i = 0; i < 5; i++) {
        lv_obj_t * item = lv_obj_create(cont);
        lv_obj_set_size(item, 40, 40);
        lv_obj_set_style_radius(item, 10, 0);
        lv_obj_set_style_border_width(item, 0, 0);
        lv_obj_set_style_bg_opa(item, 0, 0);
        lv_obj_set_style_opa(item, 150, 0);
        lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_t * img = lv_image_create(item);
        lv_image_set_src(img, my_menu[i].img_src);
        lv_obj_center(img);
        lv_obj_add_event_cb(item, item_event_cb, LV_EVENT_ALL, (void*)(intptr_t)i);
        lv_group_add_obj(lv_group_get_default(), item);
    }
}
