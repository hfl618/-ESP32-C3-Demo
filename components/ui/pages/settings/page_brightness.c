#include "../../ui_entry.h"

static lv_obj_t * lbl_val;

static void slider_event_cb(lv_event_t * e) {
    lv_obj_t * slider = lv_event_get_target(e);
    int v = (int)lv_slider_get_value(slider);
    ui_get_state()->brightness = v;
    ui_port_set_brightness(v);
    
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", v);
    lv_label_set_text(lbl_val, buf);
}

static void brightness_back_cb(lv_event_t * e) {
    if(lv_event_get_code(e) == LV_EVENT_DOUBLE_CLICKED) ui_change_page(UI_PAGE_DISPLAY);
}

void page_brightness_init(lv_obj_t * parent) {
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cont, 8, 0);

    // 1. 标题
    lv_obj_t * title = lv_label_create(cont);
    lv_label_set_text(title, "Brightness");
    lv_obj_set_style_text_font(title, ui_get_app_font(), 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);

    // 2. 加厚滑动条
    lv_obj_t * slider = lv_slider_create(cont);
    lv_obj_set_size(slider, 120, 12);
    lv_slider_set_value(slider, ui_get_state()->brightness, LV_ANIM_OFF);
    
    // 样式：深色槽 + 亮黄进度
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x2A2A2A), LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0xFFCC00), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(slider, 0, LV_PART_KNOB); // 隐藏旋钮
    lv_obj_add_flag(slider, LV_OBJ_FLAG_EVENT_BUBBLE);
    
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_group_add_obj(lv_group_get_default(), slider);

    // 3. 百分比数值
    lbl_val = lv_label_create(cont);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", ui_get_state()->brightness);
    lv_label_set_text(lbl_val, buf);
    lv_obj_set_style_text_color(lbl_val, lv_color_hex(0xFFCC00), 0);

    lv_obj_add_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(cont, brightness_back_cb, LV_EVENT_DOUBLE_CLICKED, NULL);
}
