/**
 * @file page_fontsize.c
 * @brief 全局字体大小设置页面
 */

#include "../../ui_entry.h"

/**
 * @brief 字体大小选择按钮回调
 */
static void size_btn_event_cb(lv_event_t * e) {
    int id = (intptr_t)lv_event_get_user_data(e);
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        // 1. 调用 UI 管理器更新字体（UI 内部样式逻辑）
        ui_set_app_font_size(id);
        
        // 2. 刷新当前页面以展示选中效果
        ui_change_page(UI_PAGE_FONTSIZE); 
    }
}

void page_fontsize_init(lv_obj_t * parent) {
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cont, 4, 0);

    // 标题
    lv_obj_t * title = lv_label_create(cont);
    lv_label_set_text(title, "Font Size");
    lv_obj_set_style_text_color(title, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_12, 0);

    // 水平选择组
    lv_obj_t * btn_group = lv_obj_create(cont);
    lv_obj_set_size(btn_group, 150, 32);
    lv_obj_set_style_bg_opa(btn_group, 0, 0);
    lv_obj_set_style_border_width(btn_group, 0, 0);
    lv_obj_set_flex_flow(btn_group, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_group, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(btn_group, 3, 0); 

    const char * labels[] = {"S", "M", "L"};
    int current_level = ui_get_state()->font_size_level;

    for(int i = 0; i < 3; i++) {
        lv_obj_t * btn = lv_btn_create(btn_group);
        lv_obj_set_size(btn, 46, 24);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x1F1F1F), 0);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x333333), LV_STATE_FOCUSED);
        lv_obj_set_style_border_width(btn, 2, LV_STATE_FOCUSED);
        lv_obj_set_style_border_color(btn, lv_color_white(), LV_STATE_FOCUSED);
        lv_obj_set_style_radius(btn, 4, 0);

        // 高亮当前选中项
        if(i == current_level) {
            lv_obj_set_style_border_width(btn, 1, 0);
            lv_obj_set_style_border_color(btn, lv_palette_main(LV_PALETTE_BLUE), 0);
        }

        lv_obj_t * lbl = lv_label_create(btn);
        lv_label_set_text(lbl, labels[i]);
        lv_obj_center(lbl);
        
        lv_obj_add_event_cb(btn, size_btn_event_cb, LV_EVENT_CLICKED, (void*)(intptr_t)i);
        lv_group_add_obj(lv_group_get_default(), btn);
    }
}
