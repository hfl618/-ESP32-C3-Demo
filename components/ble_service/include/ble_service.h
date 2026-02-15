#ifndef __BLE_SERVICE_H__
#define __BLE_SERVICE_H__

#include "esp_err.h"
#include <stdbool.h>

/* 初始化蓝牙协议栈（仅加载，不开启广播） */
esp_err_t ble_service_init(void);

/* 开启蓝牙广播 */
void ble_service_on(void);

/* 彻底关闭蓝牙广播 */
void ble_service_off(void);

#endif
