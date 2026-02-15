/**
 * @file page_volume.c
 * @brief 音量调节页面
 * 
 * 样式源码：负责展示音量滑动条及百分比数值。
 * 逻辑解耦：通过信号总线发送音量变化请求，不直接控制硬件。
 */

#include "../../ui_entry.h"

static lv_obj_t * lbl_vol_val;

/**
 * @brief 音量页面事件回调
 */
static void volume_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    ui_state_t * state = ui_get_state();
    
    // 情况 A：滑动条数值改变（由滚轮驱动）
    if(code == LV_EVENT_VALUE_CHANGED) {
        int val = (int)lv_slider_get_value(obj);
        
        // 1. 更新 UI 内部状态
        state->volume = val;
        state->is_dirty = true; // 【新增】标记数据已脏
        
        char buf[8];
        lv_snprintf(buf, sizeof(buf), "%d%%", val);
        lv_label_set_text(lbl_vol_val, buf);
        
        // 2. 模块化投递信号
        ui_post_event(UI_EVT_SET_VOLUME, (intptr_t)val); 
    }
    // 情况 B：点击背景（或容器）
    else if(code == LV_EVENT_CLICKED) {
        // 返回设置主页面
        ui_change_page(UI_PAGE_SETTINGS);
    }
}

/**
 * @brief 音量页面初始化
 */
void page_volume_init(lv_obj_t * parent) {
    // 1. 创建页面主容器
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cont, 8, 0);

    // 2. 标题展示
    lv_obj_t * title = lv_label_create(cont);
    lv_label_set_text(title, LV_SYMBOL_VOLUME_MAX " Volume");
    lv_obj_set_style_text_font(title, ui_get_app_font(), 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);

    // 3. 音量滑动条
    lv_obj_t * slider = lv_slider_create(cont);
    lv_obj_set_size(slider, 120, 12);
    // 从全局状态获取初始值
    lv_slider_set_value(slider, ui_get_state()->volume, LV_ANIM_OFF);
    
    // 滑动条样式定制
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, lv_palette_main(LV_PALETTE_CYAN), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(slider, 0, LV_PART_KNOB); // 隐藏旋钮，适应滚轮操作
    
    // 注册事件并加入编码器控制组
    lv_obj_add_event_cb(slider, volume_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_group_add_obj(lv_group_get_default(), slider);
    
    // --- 核心修复：进入页面即自动聚焦滑块并开启编辑模式 ---
    lv_group_t * g = lv_group_get_default();
    lv_group_focus_obj(slider);
    lv_group_set_editing(g, true); // 关键：强制进入编辑模式，开门即可转动调节

    // 4. 百分比数值标签
    lbl_vol_val = lv_label_create(cont);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", ui_get_state()->volume);
    lv_label_set_text(lbl_vol_val, buf);
    lv_obj_set_style_text_font(lbl_vol_val, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_vol_val, lv_palette_main(LV_PALETTE_CYAN), 0);

    // 5. 交互增强：点击背景返回
    lv_obj_add_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(cont, volume_event_cb, LV_EVENT_CLICKED, NULL);
}
