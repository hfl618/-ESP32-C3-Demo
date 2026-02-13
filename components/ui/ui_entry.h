/**
 * @file ui_entry.h
 * @brief UI 系统的全局接口定义
 * 
 * 职责：
 * 1. 定义页面索引枚举，作为页面切换的标识。
 * 2. 声明 UI 管理器、状态栏及各页面的公有接口。
 * 3. 屏蔽底层实现细节，提供统一的操作函数。
 */

#ifndef UI_ENTRY_H
#define UI_ENTRY_H

#include "lvgl.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* =============================================
 * 1. 页面标识枚举
 * ============================================= */
typedef enum {
    UI_PAGE_MAIN,      /**< 主菜单滚动页面 */
    UI_PAGE_SETTINGS,  /**< 设置列表页面 */
} ui_page_t;

/* =============================================
 * 2. 核心调度接口 (由 ui_manager.c 实现)
 * ============================================= */

/**
 * @brief UI 系统初始化入口
 * 在 main.c 中调用，负责创建全局背景、状态栏和加载初始页面。
 */
void ui_init(void);

/**
 * @brief 全局页面切换函数
 * @param page 目标页面的枚举标识
 * 此函数会自动清空旧页面、释放内存并重置编码器焦点组。
 */
void ui_change_page(ui_page_t page);

/* =============================================
 * 3. 状态栏控制接口 (由 ui_status_bar.c 实现)
 * ============================================= */

/**
 * @brief 创建静态状态栏
 * @param parent 状态栏的父容器 (通常是屏幕对象)
 */
void ui_status_bar_create(lv_obj_t * parent);

/**
 * @brief 更新电池电量显示
 * @param level 电量百分比 (0-100)
 * 包含低电量自动变红及最小宽度显示补偿逻辑。
 */
void ui_battery_set_level(int level);

/**
 * @brief 设置 WiFi 连接状态
 * @param connected true: 显示蓝色图标; false: 显示红色图标并带感叹号
 */
void ui_status_bar_set_wifi_conn(bool connected);

/**
 * @brief 设置蓝牙连接状态
 * @param connected true: 显示青色图标; false: 显示红色图标并带感叹号
 */
void ui_status_bar_set_bt_conn(bool connected);

/* =============================================
 * 4. 页面构建内部接口 (由各 page_xxx.c 实现)
 * ============================================= */

/**
 * @brief 主页面工厂函数
 * @param parent 页面内容容器
 */
void page_main_init(lv_obj_t * parent);

/**
 * @brief 设置页面工厂函数
 * @param parent 页面内容容器
 */
void page_settings_init(lv_obj_t * parent);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_ENTRY_H*/
