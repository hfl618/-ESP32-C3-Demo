/**
 * @file sys_msg.c
 * @brief 系统消息总线核心底座
 * 
 * 职责：作为全项目唯一的、跨任务的消息投递内核。
 * 所有模块（WiFi、蓝牙、UI、传感器）产生的信号均汇聚于此，
 * 并通过 FreeRTOS 队列分发到 GUI 引擎或其他监控者。
 */

#include "include/sys_msg.h"
#include "esp_log.h"

static const char *TAG = "SYS_MSG";

QueueHandle_t g_sys_msg_queue = NULL;

/**
 * @brief 全局消息投递函数
 * 
 * @param source 消息来源（如 MSG_SOURCE_UI, MSG_SOURCE_WIFI 等）
 * @param event  具体事件 ID（语义化 ID，见 sys_msg.h）
 * @param data   可选的附加数据（使用 intptr_t 或强转指针，确保不产生内存分配）
 */
void sys_msg_send(msg_source_t source, int event, void* data) {
    // 安全性检查：确保邮局已经初始化，防止系统启动初期崩溃
    if (g_sys_msg_queue == NULL) {
        ESP_LOGW(TAG, "Queue not ready! Dropping msg: %d", event);
        return;
    }

    // 封装标准信封
    sys_msg_t msg = {
        .source = source,
        .event  = event,
        .data   = data
    };

    /**
     * 全局日志：监控全系统的信号流动。
     * 在开发阶段，这是定位跨模块逻辑问题的最有效工具。
     */
    ESP_LOGI(TAG, "Post Signal: Src=%d, Evt=0x%X", source, event);

    /**
     * 投递信号。
     * 使用 0 等待时间，确保即使消息队列瞬间拥满，发信方（特别是 UI 线程）也不会卡死。
     */
    xQueueSend(g_sys_msg_queue, &msg, 0);
}
