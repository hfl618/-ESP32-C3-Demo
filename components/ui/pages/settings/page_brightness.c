/**
 * @file page_brightness.c
 * @brief 屏幕亮度调节页面
 * 
 * 职责：展示亮度滑动条。
 * 信号：通过 ui_post_event 投递 UI_EVT_SET_BRIGHTNESS 意图。
 */

#include "../../ui_entry.h"

static lv_obj_t * lbl_val;

/**
 * @brief 滑动条事件回调
 */
static void slider_event_cb(lv_event_t * e) {
    lv_obj_t * slider = lv_event_get_target(e);
    int v = (int)lv_slider_get_value(slider);
    ui_state_t * state = ui_get_state();
    
    // 1. 更新 UI 内部状态
    state->brightness = v;
    state->is_dirty = true; // 【新增】标记数据已脏，需要保存
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", v);
    lv_label_set_text(lbl_val, buf);
    
    // 2. 模块化投递信号：请求改变硬件亮度
    ui_post_event(UI_EVT_SET_BRIGHTNESS, (intptr_t)v);
}

void page_brightness_init(lv_obj_t * parent) {
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cont, 8, 0);

    // 标题
    lv_obj_t * title = lv_label_create(cont);
    lv_label_set_text(title, "Brightness");
    lv_obj_set_style_text_font(title, ui_get_app_font(), 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);

    // 加厚滑动条
    lv_obj_t * slider = lv_slider_create(cont);
    lv_obj_set_size(slider, 120, 12);
    lv_slider_set_value(slider, ui_get_state()->brightness, LV_ANIM_OFF);
    
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x2A2A2A), LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0xFFCC00), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(slider, 0, LV_PART_KNOB); 
    
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_group_add_obj(lv_group_get_default(), slider);
    
    // --- 核心修复：进入页面即自动聚焦并开启编辑模式 ---
    lv_group_t * g = lv_group_get_default();
    lv_group_focus_obj(slider);
    lv_group_set_editing(g, true);

    // 数值显示
    lbl_val = lv_label_create(cont);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", ui_get_state()->brightness);
    lv_label_set_text(lbl_val, buf);
    lv_obj_set_style_text_color(lbl_val, lv_color_hex(0xFFCC00), 0);
}
