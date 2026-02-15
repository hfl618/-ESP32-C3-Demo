#include "../../ui_entry.h"

static void size_btn_event_cb(lv_event_t * e) {
    int id = (intptr_t)lv_event_get_user_data(e);
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        ui_set_app_font_size(id);
        ui_change_page(UI_PAGE_FONTSIZE); // 刷新页面以显示选中效果
    }
}

static void fontsize_back_cb(lv_event_t * e) {
    if(lv_event_get_code(e) == LV_EVENT_DOUBLE_CLICKED) {
        ui_change_page(UI_PAGE_DISPLAY);
    }
}

void page_fontsize_init(lv_obj_t * parent) {
    // 1. 创建主容器，彻底禁用滚动功能
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    
    // 关键修正：禁用滚动并隐藏滚动条轨迹
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cont, 4, 0);

    // 2. 标题
    lv_obj_t * title = lv_label_create(cont);
    lv_label_set_text(title, "Font Size");
    lv_obj_set_style_text_color(title, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_12, 0);

    // 3. 水平按钮组
    lv_obj_t * btn_group = lv_obj_create(cont);
    lv_obj_set_size(btn_group, 150, 32);
    lv_obj_set_style_bg_opa(btn_group, 0, 0);
    lv_obj_set_style_border_width(btn_group, 0, 0);
    lv_obj_set_style_pad_all(btn_group, 0, 0);
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
        lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);

        // --- 核心修复：根据全局状态高亮当前选中项 ---
        if(i == current_level) {
            lv_obj_set_style_border_width(btn, 1, 0);
            lv_obj_set_style_border_color(btn, lv_palette_main(LV_PALETTE_BLUE), 0);
            lv_obj_set_style_bg_color(btn, lv_color_hex(0x2A2A2A), 0);
        }

        lv_obj_t * lbl = lv_label_create(btn);
        lv_label_set_text(lbl, labels[i]);
        lv_obj_center(lbl);
        
        lv_obj_add_event_cb(btn, size_btn_event_cb, LV_EVENT_CLICKED, (void*)(intptr_t)i);
        lv_group_add_obj(lv_group_get_default(), btn);
    }

    // 4. 背景双击返回
    lv_obj_add_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(cont, fontsize_back_cb, LV_EVENT_DOUBLE_CLICKED, NULL);
}
