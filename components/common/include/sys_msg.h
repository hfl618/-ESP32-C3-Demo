#ifndef __SYS_MSG_H__
#define __SYS_MSG_H__

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/* --- 全局消息来源 --- */
typedef enum {
    MSG_SOURCE_WIFI,
    MSG_SOURCE_AI,
    MSG_SOURCE_SENSOR,
    MSG_SOURCE_UI,
} msg_source_t;

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
    UI_EVT_SET_WIFI_CONFIG,
    UI_EVT_SET_BT_CONFIG,
    UI_EVT_SET_BRIGHTNESS,
    UI_EVT_SET_SYSTEM_INFO,
} ui_evt_id_t;

/* --- 统一消息结构 --- */
typedef struct {
    msg_source_t source;
    int          event; 
    void*        data;
} sys_msg_t;

#endif
