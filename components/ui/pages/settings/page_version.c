#include "../../ui_entry.h"
#include <stdlib.h> // 用于 rand()

static bool has_new_version = true; 

static void version_page_event_cb(lv_event_t * e) {
    if(lv_event_get_code(e) == LV_EVENT_DOUBLE_CLICKED) ui_change_page(UI_PAGE_SETTINGS);
}

// 模拟真实更新判定逻辑
static void simulated_update_process_cb(lv_timer_t * t) {
    // 模拟随机结果：80% 成功，20% 失败
    bool success = (rand() % 10) < 8;

    if(success) {
        // 展示成功弹窗：自动固定为绿色勾号
        ui_show_alert("Success", "System updated to v1.2.5 successfully.", UI_ALERT_TYPE_SUCCESS, UI_PAGE_VERSION);
    } else {
        // 展示失败弹窗：自动固定为红色感叹号
        ui_show_alert("Update Failed", "Server timeout. Please check your internet connection.", UI_ALERT_TYPE_ERROR, UI_PAGE_VERSION);
    }
}

static void update_btn_event_cb(lv_event_t * e) {
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        // 1. 进入加载页
        ui_show_loading("Updating...", NULL, 0, 0);
        
        // 2. 模拟一个 2.5 秒的异步处理过程
        lv_timer_t * timer = lv_timer_create(simulated_update_process_cb, 2500, NULL);
        lv_timer_set_repeat_count(timer, 1);
    }
}

void page_version_init(lv_obj_t * parent) {
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * lbl_sys = lv_label_create(cont);
    lv_label_set_text(lbl_sys, "v1.2.4");
    lv_obj_set_style_text_font(lbl_sys, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(lbl_sys, lv_color_white(), 0);
    lv_obj_align(lbl_sys, LV_ALIGN_CENTER, -10, -8);

    if(has_new_version) {
        lv_obj_t * btn = lv_btn_create(cont);
        lv_obj_set_size(btn, 45, 18); 
        lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_set_style_radius(btn, 4, 0);
        lv_obj_align_to(btn, lbl_sys, LV_ALIGN_OUT_RIGHT_MID, 8, 0);
        lv_obj_t * btn_lbl = lv_label_create(btn);
        lv_label_set_text(btn_lbl, "New");
        lv_obj_set_style_text_font(btn_lbl, &lv_font_montserrat_10, 0);
        lv_obj_center(btn_lbl);
        lv_obj_add_event_cb(btn, update_btn_event_cb, LV_EVENT_CLICKED, NULL);
        lv_group_add_obj(lv_group_get_default(), btn);
    }

    lv_obj_t * lbl_hw = lv_label_create(cont);
    lv_label_set_text(lbl_hw, "HW: v2.0-Pro");
    lv_obj_set_style_text_font(lbl_hw, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_color(lbl_hw, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_align(lbl_hw, LV_ALIGN_CENTER, 0, 12);

    lv_obj_add_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(cont, version_page_event_cb, LV_EVENT_DOUBLE_CLICKED, NULL);
}
