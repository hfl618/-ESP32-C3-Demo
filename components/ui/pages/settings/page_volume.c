#include "../../ui_entry.h"

static lv_obj_t * lbl_vol_val;

static void volume_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * slider = lv_event_get_target(e);
    
    if(code == LV_EVENT_VALUE_CHANGED) {
        int val = (int)lv_slider_get_value(slider);
        ui_get_state()->volume = val;
        ui_port_set_volume(val); 
        
        char buf[8];
        lv_snprintf(buf, sizeof(buf), "%d%%", val);
        lv_label_set_text(lbl_vol_val, buf);
    }
    else if(code == LV_EVENT_DOUBLE_CLICKED) {
        ui_change_page(UI_PAGE_SETTINGS);
    }
}

void page_volume_init(lv_obj_t * parent) {
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cont, 8, 0);

    lv_obj_t * title = lv_label_create(cont);
    lv_label_set_text(title, LV_SYMBOL_VOLUME_MAX " Volume");
    lv_obj_set_style_text_font(title, ui_get_app_font(), 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);

    lv_obj_t * slider = lv_slider_create(cont);
    lv_obj_set_size(slider, 120, 12);
    lv_slider_set_value(slider, ui_get_state()->volume, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, lv_palette_main(LV_PALETTE_CYAN), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(slider, 0, LV_PART_KNOB);
    lv_obj_add_flag(slider, LV_OBJ_FLAG_EVENT_BUBBLE);
    
    lv_obj_add_event_cb(slider, volume_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(lv_group_get_default(), slider);

    lbl_vol_val = lv_label_create(cont);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", ui_get_state()->volume);
    lv_label_set_text(lbl_vol_val, buf);
    lv_obj_set_style_text_font(lbl_vol_val, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_vol_val, lv_palette_main(LV_PALETTE_CYAN), 0);

    lv_obj_add_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(cont, volume_event_cb, LV_EVENT_DOUBLE_CLICKED, NULL);
}
