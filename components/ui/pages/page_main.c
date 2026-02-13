/**
 * @file page_main.c
 * @brief 主菜单页面构建
 * 
 * 核心逻辑：
 * 1. 采用 Flex Row 布局实现横向滚动。
 * 2. 通过事件回调处理选中时的精致“呼吸感”圆环动画。
 * 3. 集成编码器组管理。
 */

#include "../ui_entry.h"
#include "esp_log.h"

/* 声明外部自定义图标 */
LV_IMAGE_DECLARE(settings);
LV_IMAGE_DECLARE(wifi);
LV_IMAGE_DECLARE(music);
LV_IMAGE_DECLARE(message_ai);

/* 菜单数据静态定义 */
static const struct {
    const lv_image_dsc_t * img_src;
} my_menu[] = {
    {&settings},
    {&wifi},
    {&music},
    {&message_ai},
    {&settings} // 占位
};

/**
 * @brief 图标项事件回调
 */
static void item_event_cb(lv_event_t * e) {
    lv_obj_t * obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    int index = (intptr_t)lv_event_get_user_data(e);

    if(code == LV_EVENT_FOCUSED || code == LV_EVENT_CLICKED || code == LV_EVENT_PRESSED) {
        ESP_LOGI("UI_MAIN", "Item %d event: %d", index, code);
    }

    if(code == LV_EVENT_FOCUSED) {
        // 选中：显示圆角背景 + 精致圆环
        lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x333333), 0); 
        
        lv_obj_set_style_outline_width(obj, 2, 0);      // 2px 精致圆环
        lv_obj_set_style_outline_color(obj, lv_color_white(), 0);
        lv_obj_set_style_outline_pad(obj, 2, 0);        // 2px 呼吸间隙
        lv_obj_set_style_outline_opa(obj, 180, 0);      
        
        lv_obj_set_style_opa(obj, 255, 0);            // 变亮
        lv_obj_scroll_to_view(obj, LV_ANIM_ON);       // 居中显示
    }
    else if(code == LV_EVENT_DEFOCUSED) {
        // 未选中：关闭背景和圆环，图标变暗
        lv_obj_set_style_bg_opa(obj, 0, 0);
        lv_obj_set_style_outline_width(obj, 0, 0);
        lv_obj_set_style_opa(obj, 150, 0); 
    }
    else if(code == LV_EVENT_PRESSED) {
        // 按下时稍微缩小并加深背景，提供点击反馈
        lv_obj_set_style_transform_scale(obj, 240, 0); // 稍微缩小 (256 is 100%)
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x111111), 0);
    }
    else if(code == LV_EVENT_RELEASED || code == LV_EVENT_CANCEL) {
        // 释放时恢复
        lv_obj_set_style_transform_scale(obj, 256, 0);
        if(lv_obj_has_state(obj, LV_STATE_FOCUSED)) {
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x333333), 0);
        }
    }
    else if(code == LV_EVENT_CLICKED) {
        ESP_LOGI("UI_MAIN", "Clicked index: %d", index);
        if(index == 0) ui_change_page(UI_PAGE_SETTINGS);
        else {
            // 提示用户该功能未实现，但证明了按键是有效的
            ESP_LOGW("UI_MAIN", "Function for item %d not implemented", index);
        }
    }
}

/**
 * @brief 构建主页面内容
 */
void page_main_init(lv_obj_t * parent) {
    // 1. 滚动容器初始化
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, 160, 62);
    lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    
    // 2. Flex 布局配置
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    lv_obj_set_style_pad_left(cont, 60, 0);
    lv_obj_set_style_pad_right(cont, 60, 0);
    lv_obj_set_style_pad_column(cont, 25, 0);

    lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    // 3. 循环创建 5 个自定义图标项
    for(uint32_t i = 0; i < 5; i++) {
        lv_obj_t * item = lv_obj_create(cont);
        lv_obj_set_size(item, 40, 40);
        lv_obj_set_style_radius(item, 10, 0);
        lv_obj_set_style_border_width(item, 0, 0);
        lv_obj_set_style_bg_opa(item, 0, 0);
        lv_obj_set_style_opa(item, 150, 0);
        lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);

        // 创建自定义图标镜像
        lv_obj_t * img = lv_image_create(item);
        lv_image_set_src(img, my_menu[i].img_src);
        lv_obj_center(img);

        // 绑定事件
        lv_obj_add_event_cb(item, item_event_cb, LV_EVENT_ALL, (void*)(intptr_t)i);
        lv_group_add_obj(lv_group_get_default(), item);
    }
}
