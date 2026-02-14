/**
 * @file ui_manager.c
 * @brief 页面总调度器实现
 * 
 * 原理：
 * 1. 采用单页面应用 (SPA) 模式。
 * 2. 状态栏 (Status Bar) 是静态的，一旦创建永不销毁。
 * 3. 内容区 (Content Area) 是动态的，切换页面时先 lv_obj_clean，再调用工厂函数。
 * 4. 统一管理 LVGL 默认组 (Group)，确保编码器操作一致性。
 */

#include "ui_entry.h"
#include "esp_log.h"

/* 内容区域容器句柄 */
static lv_obj_t * content_area = NULL;

/**
 * @brief 执行页面切换
 * 逻辑：清理旧页面 -> 清理组 -> 加载新页面
 */
void ui_change_page(ui_page_t page) {
    if(!content_area) return;

    // 1. 核心：清空默认组中的所有对象。
    // 如果不清理，转动编码器时会尝试聚焦已经消失的旧对象，导致系统奔溃。
    lv_group_remove_all_objs(lv_group_get_default());

    // 2. 销毁 content_area 下的所有子对象 (自动释放内存)
    lv_obj_clean(content_area);

    // 3. 根据目标枚举调用对应的工厂函数
    switch(page) {
        case UI_PAGE_MAIN:
            page_main_init(content_area);
            break;
        case UI_PAGE_SETTINGS:
            page_settings_init(content_area);
            break;
    }

    // 修复编码器：强制聚焦新页面的第一个对象
    lv_obj_t * first_child = lv_obj_get_child(content_area, 0);
    if(first_child) {
        // 如果页面里还有子容器（如 page_main 里的 cont），则聚焦子容器的第一个孩子
        if(lv_obj_get_child_count(first_child) > 0) {
            lv_obj_t * target = lv_obj_get_child(first_child, 0);
            lv_group_focus_obj(target);
            lv_obj_scroll_to_view(target, LV_ANIM_OFF);
        } else {
            lv_group_focus_obj(first_child);
            lv_obj_scroll_to_view(first_child, LV_ANIM_OFF);
        }
    }
}

/**
 * @brief UI 系统初始化入口
 */
void ui_init(void) {
    lv_obj_t * screen = lv_screen_active();
    
    // 设置全局屏幕背景颜色
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    // A. 构建顶层静态结构：状态栏
    ui_status_bar_create(screen);

    // B. 构建底层动态结构：全屏页面容器 (高度扣除状态栏后的剩余部分)
    content_area = lv_obj_create(screen);
    lv_obj_set_size(content_area, 160, 62);
    lv_obj_align(content_area, LV_ALIGN_BOTTOM_MID, 0, 0);
    
    // 样式重置：必须透明且无边框，否则切换时会有脏视觉残留
    lv_obj_set_style_bg_opa(content_area, 0, 0);
    lv_obj_set_style_border_width(content_area, 0, 0);
    lv_obj_set_style_pad_all(content_area, 0, 0);

    // C. 初始加载主菜单页面
    ui_change_page(UI_PAGE_MAIN);

    // D. 模拟系统初始业务状态 (生产环境中应由底层驱动触发)
    ui_battery_set_level(100);         // 测试：红色低电量
    //ui_status_bar_set_wifi_conn(true); // 测试：蓝色 WiFi 已连
    //ui_status_bar_set_bt_conn(false); // 测试：红色蓝牙未连
}
