/**
 * @file page_version.c
 * @brief 系统版本展示与更新模拟页面
 * 
 * 职责：展示固件/硬件版本号，模拟 OTA 更新流程。
 * 信号：通过 ui_show_loading 和 ui_show_alert 模块化反馈状态。
 */

#include "../../ui_entry.h"
#include <stdlib.h> 

static bool has_new_version = true; 

/**
 * @brief 模拟真实更新判定逻辑的回调（工程模拟）
 */
static void simulated_update_process_cb(lv_timer_t * t) {
    // 模拟随机结果：80% 成功，20% 失败
    bool success = (rand() % 10) < 8;

    if(success) {
        // 展示成功弹窗：自动跳转回当前页
        ui_show_alert("Success", "System updated to v1.2.5 successfully.", UI_ALERT_TYPE_SUCCESS, UI_PAGE_VERSION);
    } else {
        // 展示失败弹窗
        ui_show_alert("Update Failed", "Server timeout. Please check your internet connection.", UI_ALERT_TYPE_ERROR, UI_PAGE_VERSION);
    }
}

/**
 * @brief 更新按钮点击事件
 */
static void update_btn_event_cb(lv_event_t * e) {
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        // 1. 调用模块化加载页（不含逻辑，仅样式展示）
        ui_show_loading("Updating...", NULL, 0, 0);
        
        // 2. 开启模拟计时器（此处仅为演示 UI 反馈）
        lv_timer_t * timer = lv_timer_create(simulated_update_process_cb, 2500, NULL);
        lv_timer_set_repeat_count(timer, 1);
    }
}

/**
 * @brief 版本页面初始化
 */
void page_version_init(lv_obj_t * parent) {
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    // 系统版本号
    lv_obj_t * lbl_sys = lv_label_create(cont);
    lv_label_set_text(lbl_sys, "v1.2.4");
    // 修正：使用项目中已启用的 14 号字体代替 16 号
    lv_obj_set_style_text_font(lbl_sys, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_sys, lv_color_white(), 0);
    lv_obj_align(lbl_sys, LV_ALIGN_CENTER, -10, -8);

    // 如果有新版本，显示 "New" 标签按钮
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
        
        // 注册点击事件并加入编码器组
        lv_obj_add_event_cb(btn, update_btn_event_cb, LV_EVENT_CLICKED, NULL);
        lv_group_add_obj(lv_group_get_default(), btn);
    }

    // 硬件版本号
    lv_obj_t * lbl_hw = lv_label_create(cont);
    lv_label_set_text(lbl_hw, "HW: v2.0-Pro");
    lv_obj_set_style_text_font(lbl_hw, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_color(lbl_hw, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_align(lbl_hw, LV_ALIGN_CENTER, 0, 12);

    /**
     * 注意：
     * 这里不再手动绑定双击事件。
     * 双击返回逻辑已在底层由 gui_engine 全局接管。
     */
}
