#include "ui_entry.h"

static lv_obj_t * root_container = NULL;
static lv_obj_t * content_area = NULL;
static const lv_font_t * current_font = &lv_font_montserrat_12;

extern const lv_image_dsc_t message_ai; 

static lv_obj_t * active_loading_label = NULL;
static lv_obj_t * active_alert_label = NULL;

static struct {
    const char * text;
    const void * icon;
    uint32_t ms;
    ui_page_t next;
    const char * alert_title;
    ui_alert_type_t alert_type;
} sys_ctx;

static ui_state_t global_ui_state = { .wifi_en=false, .bt_en=false, .status_bar_en=true, .brightness=80, .volume=50, .font_size_level=1 };
ui_state_t * ui_get_state(void) { return &global_ui_state; }

static void set_smart_label_text(lv_obj_t * label, const char * text) {
    if(!label) return;
    lv_label_set_text(label, text);
    lv_obj_update_layout(label);
    if(lv_obj_get_width(label) >= 140) {
        lv_obj_set_width(label, lv_pct(90));
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    } else {
        lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    }
}

const lv_font_t * ui_get_app_font(void) { return current_font; }
void ui_set_app_font_size(int level) {
    global_ui_state.font_size_level = level;
    if(level == 0) current_font = &lv_font_montserrat_10;
    else if(level == 1) current_font = &lv_font_montserrat_12;
    else if(level == 2) current_font = &lv_font_montserrat_14;
}

static void loading_timer_cb(lv_timer_t * t) { ui_change_page(sys_ctx.next); }
static void internal_page_loading_init(lv_obj_t * p) {
    lv_obj_set_size(p, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(p, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(p, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(p, 5, 0);
    if(!sys_ctx.icon) {
        lv_obj_t * s = lv_spinner_create(p);
        lv_obj_set_size(s, 25, 25);
        lv_obj_set_style_arc_color(s, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);
    } else {
        lv_obj_t * img = lv_image_create(p);
        lv_image_set_src(img, sys_ctx.icon);
    }
    active_loading_label = lv_label_create(p);
    lv_obj_set_style_text_font(active_loading_label, current_font, 0);
    set_smart_label_text(active_loading_label, sys_ctx.text);
    if(sys_ctx.ms > 0) {
        lv_timer_t * timer = lv_timer_create(loading_timer_cb, sys_ctx.ms, NULL);
        lv_timer_set_repeat_count(timer, 1);
    }
}

static void alert_click_cb(lv_event_t * e) { ui_change_page(sys_ctx.next); }
static void internal_page_alert_init(lv_obj_t * p) {
    lv_obj_set_size(p, lv_pct(100), lv_pct(100));
    lv_obj_clear_flag(p, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(p, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(p, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_top(p, 6, 0);
    lv_obj_set_style_pad_row(p, 4, 0);

    lv_obj_t * h = lv_obj_create(p);
    lv_obj_set_size(h, lv_pct(95), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(h, 0, 0);
    lv_obj_set_style_border_width(h, 0, 0);
    lv_obj_set_style_pad_all(h, 0, 0);
    lv_obj_set_flex_flow(h, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(h, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(h, 8, 0);

    if(sys_ctx.icon) {
        lv_obj_t * img = lv_image_create(h);
        lv_image_set_src(img, sys_ctx.icon);
    } else {
        if(sys_ctx.alert_type == UI_ALERT_TYPE_AI) {
            // --- 核心：AI 专用头像 ---
            lv_obj_t * img = lv_image_create(h);
            lv_image_set_src(img, &message_ai);
        } else {
            lv_obj_t * i = lv_label_create(h);
            if(sys_ctx.alert_type == UI_ALERT_TYPE_SUCCESS) {
                lv_label_set_text(i, LV_SYMBOL_OK);
                lv_obj_set_style_text_color(i, lv_palette_main(LV_PALETTE_GREEN), 0);
            } else if(sys_ctx.alert_type == UI_ALERT_TYPE_ERROR) {
                lv_label_set_text(i, LV_SYMBOL_CLOSE);
                lv_obj_set_style_text_color(i, lv_palette_main(LV_PALETTE_RED), 0);
            } else {
                lv_label_set_text(i, LV_SYMBOL_WARNING);
                lv_obj_set_style_text_color(i, lv_palette_main(LV_PALETTE_BLUE), 0);
            }
            lv_obj_set_style_text_font(i, &lv_font_montserrat_14, 0);
        }
    }

    lv_obj_t * t = lv_label_create(h);
    lv_label_set_text(t, sys_ctx.alert_title);
    lv_obj_set_style_text_font(t, current_font, 0);
    lv_obj_set_style_text_color(t, lv_color_white(), 0);

    active_alert_label = lv_label_create(p);
    lv_obj_set_style_text_color(active_alert_label, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_style_text_font(active_alert_label, current_font, 0);
    lv_obj_set_width(active_alert_label, lv_pct(90));
    set_smart_label_text(active_alert_label, sys_ctx.text);

    lv_obj_add_flag(p, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(p, alert_click_cb, LV_EVENT_CLICKED, NULL);
}

void ui_show_loading(const char * text, const void * icon, uint32_t duration, ui_page_t target) {
    sys_ctx.text = text; sys_ctx.icon = icon; sys_ctx.ms = duration; sys_ctx.next = target;
    ui_change_page(UI_PAGE_LOADING);
}
void ui_update_loading_text(const char * text) { if(active_loading_label) set_smart_label_text(active_loading_label, text); }
void ui_finish_loading(ui_page_t target) { ui_change_page(target); }
void ui_show_alert(const char * title, const char * msg, ui_alert_type_t type, ui_page_t target) {
    sys_ctx.alert_title = title; sys_ctx.text = msg; sys_ctx.alert_type = type; sys_ctx.next = target; sys_ctx.icon = NULL;
    ui_change_page(UI_PAGE_ALERT);
}
void ui_show_alert_with_icon(const char * title, const char * msg, const void * icon_src, ui_page_t target) {
    sys_ctx.alert_title = title; sys_ctx.text = msg; sys_ctx.icon = icon_src; sys_ctx.next = target;
    ui_change_page(UI_PAGE_ALERT);
}
void ui_update_alert_text(const char * msg) { if(active_alert_label) set_smart_label_text(active_alert_label, msg); }

void ui_change_page(ui_page_t page) {
    if(!content_area) return;
    lv_group_remove_all_objs(lv_group_get_default());
    lv_obj_clean(content_area);
    lv_obj_set_scrollbar_mode(content_area, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_pad_all(content_area, 0, 0);
    lv_obj_scroll_to_y(content_area, 0, LV_ANIM_OFF);
    lv_obj_set_flex_flow(content_area, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(content_area, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    active_loading_label = NULL; active_alert_label = NULL;
    switch(page) {
        case UI_PAGE_MAIN:      page_main_init(content_area); break;
        case UI_PAGE_SETTINGS:  page_settings_init(content_area); break;
        case UI_PAGE_WIFI:      page_wifi_init(content_area); break;
        case UI_PAGE_WIFI_INFO: page_wifi_info_init(content_area); break;
        case UI_PAGE_VERSION:   page_version_init(content_area); break;
        case UI_PAGE_PROVISION: page_provision_init(content_area); break;
        case UI_PAGE_DISPLAY:   page_display_init(content_area); break;
        case UI_PAGE_BRIGHTNESS: page_brightness_init(content_area); break;
        case UI_PAGE_FONTSIZE:   page_fontsize_init(content_area); break;
        case UI_PAGE_VOLUME:     page_volume_init(content_area); break;
        case UI_PAGE_LOADING:    internal_page_loading_init(content_area); break;
        case UI_PAGE_ALERT:      internal_page_alert_init(content_area); break;
    }
    lv_obj_t * first = lv_obj_get_child(content_area, 0);
    if(first) lv_group_focus_obj(lv_obj_get_child_count(first) > 0 ? lv_obj_get_child(first, 0) : first);
}

void ui_init(void) {
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    root_container = lv_obj_create(scr);
    lv_obj_set_size(root_container, 160, 80);
    lv_obj_set_style_bg_opa(root_container, 0, 0);
    lv_obj_set_style_border_width(root_container, 0, 0);
    lv_obj_set_style_pad_all(root_container, 0, 0);
    lv_obj_set_style_pad_row(root_container, 0, 0);
    lv_obj_clear_flag(root_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(root_container, LV_FLEX_FLOW_COLUMN);
    ui_status_bar_create(root_container);
    content_area = lv_obj_create(root_container);
    lv_obj_set_width(content_area, 160);
    lv_obj_set_flex_grow(content_area, 1);
    lv_obj_set_style_bg_opa(content_area, 0, 0);
    lv_obj_set_style_border_width(content_area, 0, 0);
    lv_obj_set_style_pad_all(content_area, 0, 0);
    lv_obj_set_scrollbar_mode(content_area, LV_SCROLLBAR_MODE_OFF);
    ui_change_page(UI_PAGE_MAIN);
}
