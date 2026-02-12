/**
 * @file page_main.c
 * @brief 主菜单页面构建
 * 
 * 核心逻辑：
 * 1. 采用 Flex Row 布局实现横向滚动。
 * 2. 通过事件回调处理选中时的“Outline 光环”动画。
 * 3. 集成编码器组管理。
 */

#include "../ui_entry.h"

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
    {&settings} // 暂时占位
};

/**
 * @brief 图标项事件回调
 * 处理：焦点获得（变亮、加背景/外廓）、失去（变暗/透明）、点击（切页）。
 */
static void item_event_cb(lv_event_t * e) {
    lv_obj_t * obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    int index = (intptr_t)lv_event_get_user_data(e);

    if(code == LV_EVENT_FOCUSED) {
        // 选中：显示背景，白色外廓
        lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x333333), 0); // 统一使用深灰背景
        
        lv_obj_set_style_outline_width(obj, 4, 0);
        lv_obj_set_style_outline_color(obj, lv_color_white(), 0);
        lv_obj_set_style_outline_pad(obj, 2, 0);
        lv_obj_set_style_outline_opa(obj, 150, 0);
        
        lv_obj_set_style_opa(obj, 255, 0);            // 整体完全可见
        lv_obj_scroll_to_view(obj, LV_ANIM_ON);
    }
    else if(code == LV_EVENT_DEFOCUSED) {
        // 未选中：背景变透明，关闭外廓
        lv_obj_set_style_bg_opa(obj, 0, 0);
        lv_obj_set_style_outline_width(obj, 0, 0);
        lv_obj_set_style_opa(obj, 150, 0); // 未选中时图标稍微半透明，拉开层次感
    }
    else if(code == LV_EVENT_CLICKED) {
        // 点击处理：如果是第一个图标 (设置)，切换到设置页面
        if(index == 0) ui_change_page(UI_PAGE_SETTINGS);
    }
}

/**
 * @brief 构建主页面内容
 * @param parent 页面容器
 */
void page_main_init(lv_obj_t * parent) {
    // 1. 滚动容器初始化
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, 160, 62);
    lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    
    // 2. Flex 布局配置：居中对齐
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // 关键 Padding：(160-40)/2 = 60，确保首尾图标能停在中心
    lv_obj_set_style_pad_left(cont, 60, 0);
    lv_obj_set_style_pad_right(cont, 60, 0);
    lv_obj_set_style_pad_column(cont, 25, 0);

    // 物理捕捉：停止滚动时自动对齐到图标中心
    lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    // 3. 循环创建 5 个功能图标
    for(uint32_t i = 0; i < 5; i++) {
        lv_obj_t * item = lv_obj_create(cont);
        lv_obj_set_size(item, 40, 40);
        lv_obj_set_style_radius(item, 10, 0);
        
        // 基础样式
        lv_obj_set_style_radius(item, 10, 0);
        lv_obj_set_style_border_width(item, 0, 0);
        lv_obj_set_style_bg_opa(item, 0, 0);       // 初始背景透明
        lv_obj_set_style_opa(item, 150, 0);        // 初始图标半透明
        lv_obj_clear_flag(item, LV_OBJ_FLAG_SCROLLABLE);

        // 统一创建图标
        lv_obj_t * img = lv_image_create(item);
        lv_image_set_src(img, my_menu[i].img_src);
        lv_obj_center(img);

        // 绑定事件、传递索引、加入编码器组
        lv_obj_add_event_cb(item, item_event_cb, LV_EVENT_ALL, (void*)(intptr_t)i);
        lv_group_add_obj(lv_group_get_default(), item);
    }
}
