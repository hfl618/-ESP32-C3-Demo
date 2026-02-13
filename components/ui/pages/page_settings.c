#include "../ui_entry.h" // 确保包含您的头文件

/**
 * @brief 列表项点击回调
 */
static void settings_item_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
   
    if(code == LV_EVENT_CLICKED) {
        // 获取绑定的用户数据（索引或ID）
        int id = (intptr_t)lv_event_get_user_data(e);
        lv_obj_scroll_to_view(obj, LV_ANIM_ON);
        // 简单的日志或逻辑处理
        if (id == 4) { // 假设 4 是 Back
            ui_change_page(UI_PAGE_MAIN);
        } else {
            // 这里处理其他设置项点击，比如弹出 Toast 或进入二级菜单
            LV_LOG_USER("Clicked Item ID: %d", id);
        }
    }
}

/**
 * @brief 创建一个美化的列表项按钮
 * @param parent 父容器
 * @param icon 图标 (LV_SYMBOL_...)
 * @param text 文本
 * @param id   事件ID
 */
static void create_menu_item(lv_obj_t * parent, const char * icon, const char * text, int id) {
    /* 1. 创建按钮基础容器 */
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_set_width(btn, lv_pct(100)); // 宽度占满父容器
    lv_obj_set_height(btn, 32);         // 设定一个舒适的高度 (160x80屏幕大约能显示2.5行)
    
    /* 2. 样式设置：默认状态（透明或深灰） */
    lv_obj_set_style_radius(btn, 8, 0);               // 圆角
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x1F1F1F), 0); // 默认深灰背景
    lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(btn, 0, 0);         // 去掉边框
    lv_obj_set_style_pad_all(btn, 0, 0);              // 清除内边距，手动布局

    /* 3. 样式设置：聚焦/选中状态（仅显示边框） */
    lv_obj_set_style_bg_opa(btn, 0, LV_STATE_FOCUSED);                // 选中时背景透明
    lv_obj_set_style_border_width(btn, 2, LV_STATE_FOCUSED);         // 2px 边框
    lv_obj_set_style_border_color(btn, lv_color_white(), LV_STATE_FOCUSED); // 白色边框
    
    /* 添加平滑过渡动画 (边框宽度) */
    static lv_style_transition_dsc_t trans;
    static const lv_style_prop_t props[] = {LV_STYLE_BORDER_WIDTH, 0};
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_ease_out, 200, 0, NULL);
    lv_obj_set_style_transition(btn, &trans, LV_STATE_FOCUSED);
    lv_obj_set_style_transition(btn, &trans, 0); // 失去焦点时也有动画

    /* 4. 内部布局：图标 + 文字 */
    // 图标
    lv_obj_t * lbl_icon = lv_label_create(btn);
    lv_label_set_text(lbl_icon, icon);
    lv_obj_align(lbl_icon, LV_ALIGN_LEFT_MID, 10, 0); // 左侧偏移10像素
    
    // 文字
    lv_obj_t * lbl_text = lv_label_create(btn);
    lv_label_set_text(lbl_text, text);
    lv_obj_align(lbl_text, LV_ALIGN_LEFT_MID, 35, 0); // 图标右侧
    // 字体建议使用 Montserrat 14 (如果启用)，否则默认
    // lv_obj_set_style_text_font(lbl_text, &lv_font_montserrat_14, 0); 

    /* 5. 事件绑定 */
    lv_obj_add_event_cb(btn, settings_item_event_cb, LV_EVENT_CLICKED, (void*)(intptr_t)id);
    
    /* 6. 加入编码器组 (物理按键控制必备) */
    lv_group_t * g = lv_group_get_default();
    if(g) lv_group_add_obj(g, btn);
}

/**
 * @brief 构建设置页面内容
 */
void page_settings_init(lv_obj_t * parent) {
    /* 1. 创建全屏主容器（带滚动的列表容器） */
    lv_obj_t * list_cont = lv_obj_create(parent);
    lv_obj_set_size(list_cont, 160, 62); // 占满全屏
    lv_obj_set_style_bg_color(list_cont, lv_color_black(), 0); // 纯黑背景
    lv_obj_set_style_border_width(list_cont, 0, 0);
    lv_obj_set_style_radius(list_cont, 0, 0);
    
    // 关键：使用 Column Flex 布局，让条目自动垂直排列
    lv_obj_set_flex_flow(list_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(list_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // 设置内边距，让列表不要贴着屏幕边缘
    lv_obj_set_style_pad_ver(list_cont, 16, 0);
    lv_obj_set_style_pad_hor(list_cont, 5, 0); // 左右留白
    lv_obj_set_style_pad_row(list_cont, 6, 0); // 条目之间的间距

    // 隐藏滚动条 (小屏幕滚动条很丑)
    lv_obj_set_scrollbar_mode(list_cont, LV_SCROLLBAR_MODE_OFF);

    // 开启 Scroll Snap (滚动捕捉)，让选中项总是尽量居中或完整显示
    lv_obj_set_scroll_snap_y(list_cont, LV_SCROLL_SNAP_CENTER);

    /* 2. 定义数据并循环创建 */
    struct {
        const char * icon;
        const char * text;
    } items[] = {
        {LV_SYMBOL_WIFI,    "WiFi"},
        {LV_SYMBOL_BLUETOOTH, "Bluetooth"},
        {LV_SYMBOL_EYE_OPEN,  "Display"},     // 亮度
        {LV_SYMBOL_SETTINGS,  "System"},      // 版本
        {LV_SYMBOL_LEFT,      "Back"}         // 返回
    };

    for(int i = 0; i < 5; i++) {
        create_menu_item(list_cont, items[i].icon, items[i].text, i);
    }
    
    /* 3. 选中第一个项目，确保进入页面时有焦点 */
    lv_obj_t * first_btn = lv_obj_get_child(list_cont, 0);
    if(first_btn) {
        lv_obj_scroll_to_view(first_btn, LV_ANIM_OFF); // 确保在视野内
        // 注意：物理按键模式下，需要手动聚焦
        // lv_group_focus_obj(first_btn); 
    }
}