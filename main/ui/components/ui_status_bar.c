/**
 * @file ui_status_bar.c
 * @brief 顶部状态栏组件实现
 * 
 * 包含：
 * 1. 时间显示。
 * 2. 电池图标（支持 1px 边框下的低电量可见度优化）。
 * 3. 动态状态图标（WiFi/蓝牙，支持“角标”式连接反馈）。
 */

#include "../ui_entry.h"

/* 静态组件句柄，仅限本文件访问 */
static lv_obj_t * wifi_icon;   /**< WiFi 主图标 */
static lv_obj_t * bt_icon;     /**< 蓝牙主图标 */
static lv_obj_t * battery_bar; /**< 电池内部进度条 */
static lv_obj_t * wifi_warn;   /**< WiFi 感叹号角标 */
static lv_obj_t * bt_warn;     /**< 蓝牙感叹号角标 */

/**
 * @brief 内部辅助：创建带独立角标的图标
 * 解决“拼接感叹号导致图标缩小”的问题。
 * @param parent 挂载容器
 * @param symbol 图标字符 (如 LV_SYMBOL_WIFI)
 * @param warn_out 输出参数，回传感叹号对象句柄以便控制显隐
 */
static lv_obj_t * create_status_icon(lv_obj_t * parent, const char * symbol, lv_obj_t ** warn_out) {
    // 1. 固定宽度容器：防止感叹号出现时推挤旁边的时间或电池
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, 15, 18);
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    // 2. 主图标：始终保持 12px 字体大小
    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, symbol);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
    lv_obj_center(label);

    // 3. 感叹号角标：独立对象，悬浮在图标右上角
    lv_obj_t * warn = lv_label_create(cont);
    lv_label_set_text(warn, "!");
    lv_obj_set_style_text_font(warn, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(warn, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_align(warn, LV_ALIGN_CENTER, 6, -4); // 微调，防止重叠时遮挡
    lv_obj_add_flag(warn, LV_OBJ_FLAG_HIDDEN);  // 默认隐藏

    *warn_out = warn;
    return label;
}

/**
 * @brief 初始化状态栏
 * 职责：构建静态背景和各子组件。
 */
void ui_status_bar_create(lv_obj_t * parent) {
    // A. 状态栏底座：高度 18px
    lv_obj_t * header = lv_obj_create(parent);
    lv_obj_set_size(header, 160, 18);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x222222), 0); // 深灰色
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_pad_all(header, 0, 0);

    // B. 时间标签：靠左对齐，白色
    lv_obj_t * time_lab = lv_label_create(header);
    lv_label_set_text(time_lab, "12:45");
    lv_obj_set_style_text_font(time_lab, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(time_lab, lv_color_white(), 0);
    lv_obj_align(time_lab, LV_ALIGN_LEFT_MID, 5, 0);

    // C. 电池组件构建
    lv_obj_t * batt_cont = lv_obj_create(header);
    lv_obj_set_size(batt_cont, 26, 12);
    lv_obj_align(batt_cont, LV_ALIGN_RIGHT_MID, -2, 0);
    lv_obj_set_style_bg_opa(batt_cont, 0, 0);
    lv_obj_set_style_border_width(batt_cont, 0, 0);
    lv_obj_set_style_pad_all(batt_cont, 0, 0);

    lv_obj_t * body = lv_obj_create(batt_cont);
    lv_obj_set_size(body, 22, 11);
    lv_obj_align(body, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_radius(body, 2, 0);
    lv_obj_set_style_bg_opa(body, 0, 0);       // 核心修复：背景设为完全透明
    lv_obj_set_style_border_width(body, 1, 0); // 仅保留 1px 白色边框
    lv_obj_set_style_border_color(body, lv_color_white(), 0);
    lv_obj_set_style_pad_all(body, 0, 0); 
    lv_obj_clear_flag(body, LV_OBJ_FLAG_SCROLLABLE);

    battery_bar = lv_bar_create(body);
    lv_obj_set_size(battery_bar, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_radius(battery_bar, 0, LV_PART_INDICATOR); // 指示器方角，避免圆角剪掉像素
    lv_obj_set_style_bg_opa(battery_bar, 0, 0);
    lv_obj_set_style_bg_color(battery_bar, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
    lv_bar_set_value(battery_bar, 100, LV_ANIM_OFF);

    lv_obj_t * tip = lv_obj_create(batt_cont);
    lv_obj_set_size(tip, 3, 5);
    lv_obj_align_to(tip, body, LV_ALIGN_OUT_RIGHT_MID, 0, 0); // 精准吸附
    lv_obj_set_style_bg_color(tip, lv_color_white(), 0);
    lv_obj_set_style_border_width(tip, 0, 0);

    // D. 状态图标池：使用 Flex 靠右排列
    lv_obj_t * icon_area = lv_obj_create(header);
    lv_obj_set_size(icon_area, 60, 18);
    lv_obj_align(icon_area, LV_ALIGN_RIGHT_MID, -32, 0);
    lv_obj_set_style_bg_opa(icon_area, 0, 0);
    lv_obj_set_style_border_width(icon_area, 0, 0);
    lv_obj_set_style_pad_all(icon_area, 0, 0);
    lv_obj_set_flex_flow(icon_area, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(icon_area, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(icon_area, 5, 0); // 设置正间距，防止重叠

    bt_icon = create_status_icon(icon_area, LV_SYMBOL_BLUETOOTH, &bt_warn);
    wifi_icon = create_status_icon(icon_area, LV_SYMBOL_WIFI, &wifi_warn);
}

/**
 * @brief 设置电池电量
 * @param level 0-100
 */
void ui_battery_set_level(int level) {
    if(!battery_bar) return;
    // 视觉补偿：低电量时，强制显示至少 15% 的宽度 (约 2-3 像素)，防止看起来像空电池
    int display_level = (level > 0 && level < 15) ? 15 : level;
    lv_bar_set_value(battery_bar, display_level, LV_ANIM_ON);

    // 低于 20% 自动变红
    if(level <= 20) lv_obj_set_style_bg_color(battery_bar, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);
    else lv_obj_set_style_bg_color(battery_bar, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
}

void ui_status_bar_set_wifi_conn(bool connected) {
    if(!wifi_icon) return;
    if(connected) {
        lv_obj_set_style_text_color(wifi_icon, lv_palette_main(LV_PALETTE_BLUE), 0);
        lv_obj_add_flag(wifi_warn, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_set_style_text_color(wifi_icon, lv_palette_main(LV_PALETTE_RED), 0);
        lv_obj_remove_flag(wifi_warn, LV_OBJ_FLAG_HIDDEN);
    }
}

void ui_status_bar_set_bt_conn(bool connected) {
    if(!bt_icon) return;
    if(connected) {
        lv_obj_set_style_text_color(bt_icon, lv_palette_main(LV_PALETTE_CYAN), 0);
        lv_obj_add_flag(bt_warn, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_set_style_text_color(bt_icon, lv_palette_main(LV_PALETTE_RED), 0);
        lv_obj_remove_flag(bt_warn, LV_OBJ_FLAG_HIDDEN);
    }
}

