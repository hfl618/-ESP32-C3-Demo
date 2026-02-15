/**
 * @file ui_status_bar.c
 * @brief 状态栏组件实现 - 增加强制布局刷新
 */

#include "../ui_entry.h"
#include <stdio.h>

static lv_obj_t * status_bar_obj = NULL;
static lv_obj_t * wifi_icon_cont;
static lv_obj_t * wifi_icon_label;
static lv_obj_t * bt_icon_cont;
static lv_obj_t * bt_icon_label;
static lv_obj_t * battery_bar;
static lv_obj_t * time_label; 

static void anim_opa_cb(void * var, int32_t v) {
    lv_obj_set_style_opa(var, v, 0);
}

static void start_blink(lv_obj_t * obj) {
    lv_anim_delete(obj, NULL);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, 255, 0);
    lv_anim_set_duration(&a, 600);
    lv_anim_set_playback_duration(&a, 600);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, anim_opa_cb);
    lv_anim_start(&a);
}

static void stop_blink(lv_obj_t * obj) {
    lv_anim_delete(obj, NULL);
    lv_obj_set_style_opa(obj, 255, 0);
}

static lv_obj_t * create_status_icon_obj(lv_obj_t * parent, const char * symbol, lv_obj_t ** label_out) {     
    lv_obj_t * cont = lv_obj_create(parent);        
    lv_obj_set_size(cont, 15, 18);
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);      
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    *label_out = lv_label_create(cont);       
    lv_label_set_text(*label_out, symbol);
    lv_obj_set_style_text_font(*label_out, &lv_font_montserrat_12, 0);
    lv_obj_center(*label_out);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_HIDDEN);
    return cont;
}

void ui_status_bar_create(lv_obj_t * parent) {      
    status_bar_obj = lv_obj_create(parent);      
    lv_obj_set_size(status_bar_obj, 160, 18);
    lv_obj_set_style_bg_color(status_bar_obj, lv_color_hex(0x222222), 0);
    lv_obj_set_style_border_width(status_bar_obj, 0, 0);    
    lv_obj_set_style_pad_all(status_bar_obj, 0, 0);
    lv_obj_set_style_radius(status_bar_obj, 0, 0);

    time_label = lv_label_create(status_bar_obj);  
    lv_label_set_text(time_label, "12:00"); 
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(time_label, lv_color_white(), 0);
    lv_obj_align(time_label, LV_ALIGN_LEFT_MID, 5, 0);

    lv_obj_t * icon_area = lv_obj_create(status_bar_obj);   
    lv_obj_set_size(icon_area, 50, 18);
    lv_obj_align(icon_area, LV_ALIGN_RIGHT_MID, -32, 0);
    lv_obj_set_style_bg_opa(icon_area, 0, 0);       
    lv_obj_set_style_border_width(icon_area, 0, 0); 
    lv_obj_set_style_pad_all(icon_area, 0, 0);      
    lv_obj_set_flex_flow(icon_area, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(icon_area, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);     
    lv_obj_set_style_pad_column(icon_area, 5, 0);

    bt_icon_cont = create_status_icon_obj(icon_area, LV_SYMBOL_BLUETOOTH, &bt_icon_label);
    wifi_icon_cont = create_status_icon_obj(icon_area, LV_SYMBOL_WIFI, &wifi_icon_label);

    lv_obj_t * batt_cont = lv_obj_create(status_bar_obj);   
    lv_obj_set_size(batt_cont, 26, 12);
    lv_obj_align(batt_cont, LV_ALIGN_RIGHT_MID, -2, 0);
    lv_obj_set_style_bg_opa(batt_cont, 0, 0);       
    lv_obj_set_style_border_width(batt_cont, 0, 0); 
    lv_obj_set_style_pad_all(batt_cont, 0, 0);      

    lv_obj_t * body = lv_obj_create(batt_cont);     
    lv_obj_set_size(body, 22, 11);
    lv_obj_align(body, LV_ALIGN_LEFT_MID, 0, 0);    
    lv_obj_set_style_radius(body, 2, 0);
    lv_obj_set_style_bg_opa(body, 0, 0);       
    lv_obj_set_style_border_width(body, 1, 0); 
    lv_obj_set_style_border_color(body, lv_color_white(), 0);
    lv_obj_set_style_pad_all(body, 1, 0);
    lv_obj_clear_flag(body, LV_OBJ_FLAG_SCROLLABLE);

    battery_bar = lv_bar_create(body);
    lv_obj_set_size(battery_bar, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_radius(battery_bar, 0, LV_PART_INDICATOR); 
    lv_obj_set_style_bg_opa(battery_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(battery_bar, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);        
    lv_bar_set_value(battery_bar, 100, LV_ANIM_OFF);

    lv_obj_t * tip = lv_obj_create(batt_cont);      
    lv_obj_set_size(tip, 3, 5);
    lv_obj_align_to(tip, body, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_color(tip, lv_color_white(), 0);
    lv_obj_set_style_border_width(tip, 0, 0);       
    lv_obj_set_style_radius(tip, 1, 0);
}

void ui_status_bar_set_visible(bool visible) {
    if(!status_bar_obj) return;
    if(visible) lv_obj_remove_flag(status_bar_obj, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(status_bar_obj, LV_OBJ_FLAG_HIDDEN);
    
    // --- 核心修复：强制父容器（root_container）重新计算 Flex 布局 ---
    lv_obj_t * root = lv_obj_get_parent(status_bar_obj);
    if(root) {
        lv_obj_update_layout(root);
        // 同时也通知内容区强制刷新
        lv_obj_t * content = lv_obj_get_child(root, 1);
        if(content) lv_obj_update_layout(content);
    }
}

void ui_status_bar_set_wifi_conn(bool connected) {  
    if(!wifi_icon_cont) return;
    stop_blink(wifi_icon_label);
    if(connected) {
        lv_obj_remove_flag(wifi_icon_cont, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_text_color(wifi_icon_label, lv_palette_main(LV_PALETTE_BLUE), 0);
    } else {
        lv_obj_add_flag(wifi_icon_cont, LV_OBJ_FLAG_HIDDEN);
    }
}

void ui_status_bar_set_wifi_connecting(void) {
    if(!wifi_icon_cont) return;
    lv_obj_remove_flag(wifi_icon_cont, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_text_color(wifi_icon_label, lv_palette_main(LV_PALETTE_BLUE), 0);
    start_blink(wifi_icon_label);
}

void ui_status_bar_set_bt_conn(bool connected) {    
    if(!bt_icon_cont) return;
    stop_blink(bt_icon_label);
    if(connected) {
        lv_obj_remove_flag(bt_icon_cont, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_style_text_color(bt_icon_label, lv_palette_main(LV_PALETTE_CYAN), 0);
    } else {
        lv_obj_add_flag(bt_icon_cont, LV_OBJ_FLAG_HIDDEN);
    }
}

void ui_status_bar_set_bt_connecting(void) {
    if(!bt_icon_cont) return;
    lv_obj_remove_flag(bt_icon_cont, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_text_color(bt_icon_label, lv_palette_main(LV_PALETTE_CYAN), 0);
    start_blink(bt_icon_label);
}

void ui_status_bar_set_time(const char *time_str) {
    if(time_label) lv_label_set_text(time_label, time_str);
}

void ui_battery_set_level(int level) {
    if(!battery_bar) return;
    lv_bar_set_value(battery_bar, level, LV_ANIM_ON);
}
