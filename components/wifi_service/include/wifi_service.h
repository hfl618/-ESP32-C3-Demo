#ifndef __WIFI_SERVICE_H__
#define __WIFI_SERVICE_H__

#include "esp_err.h"
#include "sys_msg.h" 

/* 初始化接口 (仅准备硬件，不连接) */
esp_err_t wifi_service_init(void);

/* 主动触发 Wi-Fi 连接 */
void wifi_service_connect(void);

/* 获取全局唯一的信箱句柄 */
QueueHandle_t wifi_service_get_queue(void);

#endif
