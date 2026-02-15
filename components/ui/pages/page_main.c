#include "../ui_entry.h"
#include <string.h>

LV_IMAGE_DECLARE(settings);
LV_IMAGE_DECLARE(wifi);
LV_IMAGE_DECLARE(music);
LV_IMAGE_DECLARE(message_ai);

static const struct {
    const lv_image_dsc_t * img_src;
} my_menu[] = {
    {&settings}, {&wifi}, {&music}, {&message_ai}, {&settings} 
};

static const char * full_reply = "The current temperature in London is 12°C with light rain. It's a great time for a cozy cup of tea!";
static char streaming_buffer[256];
static uint32_t char_idx = 0;

static void streaming_timer_cb(lv_timer_t * t) {
    if(char_idx < strlen(full_reply)) {
        streaming_buffer[char_idx] = full_reply[char_idx];
        char_idx++;
        streaming_buffer[char_idx] = '\0';
        ui_update_alert_text(streaming_buffer);
    } else {
        lv_timer_delete(t);
    }
}

static void start_ai_simulation(lv_timer_t * t) {
    memset(streaming_buffer, 0, sizeof(streaming_buffer));
    char_idx = 0;

    // 关键：使用自定义的机器人图标 (message_ai) 代替默认符号
    ui_show_alert_with_icon("AI Assistant", "", &message_ai, UI_PAGE_MAIN);

    lv_timer_t * stream_t = lv_timer_create(streaming_timer_cb, 50, NULL);
}

static void item_event_cb(lv_event_t * e) {
    lv_obj_t * obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    int index = (intptr_t)lv_event_get_user_data(e);

    if(code == LV_EVENT_FOCUSED) {
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
    else if(code == LV_EVENT_CLICKED) {
        if(index == 0) ui_change_page(UI_PAGE_SETTINGS);
        else if(index == 1) ui_change_page(UI_PAGE_WIFI);
        else if(index == 3) { // AI 图标
            ui_show_loading("Thinking...", NULL, 0, 0);
            lv_timer_t * delay_t = lv_timer_create(start_ai_simulation, 1500, NULL);
            lv_timer_set_repeat_count(delay_t, 1);
        }
    }
}

void page_main_init(lv_obj_t * parent) {
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100)); // 自动填充
    lv_obj_align(cont, LV_ALIGN_CENTER, 0, 0);
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
