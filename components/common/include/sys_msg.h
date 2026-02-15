#ifndef __SYS_MSG_H__
#define __SYS_MSG_H__

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"


extern QueueHandle_t g_sys_msg_queue;
/* --- 全局消息来源 --- */
typedef enum {
    MSG_SOURCE_WIFI,
    MSG_SOURCE_BT,
    MSG_SOURCE_AI,
    MSG_SOURCE_SENSOR,
    MSG_SOURCE_UI,
} msg_source_t;

/* --- Bluetooth 具体事件 --- */
typedef enum {
    BT_EVT_ADVERTISING,
    BT_EVT_CONNECTED,
    BT_EVT_DISCONNECTED,
    BT_EVT_ERROR,
} bt_evt_t;

/* --- Wi-Fi 具体事件 --- */
typedef enum {
    WIFI_EVT_CONNECTED,
    WIFI_EVT_DISCONNECTED,
    WIFI_EVT_GOT_IP,
    WIFI_EVT_TIME_SYNCED,
} wifi_evt_t;

/* --- UI 语义化意图 ID --- */
typedef enum {
    UI_EVT_MAIN_GOTO_SETTINGS,
    UI_EVT_MAIN_WIFI_CONNECT,
    UI_EVT_SET_BACK_TO_MAIN,
    UI_EVT_SET_BACK_TO_PREV,
    UI_EVT_SET_WIFI_CONFIG,
    UI_EVT_SET_BT_CONFIG,
    UI_EVT_SET_BRIGHTNESS,
    UI_EVT_SET_VOLUME,
    UI_EVT_WIFI_CONNECT_BY_IDX,
    UI_EVT_SET_SYSTEM_INFO,
    UI_EVT_SAVE_SETTINGS, // 【新增】保存设置到 NVS 的统一信号
} ui_evt_id_t;

/* --- 统一消息结构 --- */
typedef struct {
    msg_source_t source;
    int          event; 
    void*        data;
} sys_msg_t;

/* --- 通用内核函数 --- */
void sys_msg_send(msg_source_t source, int event, void* data);

#endif
