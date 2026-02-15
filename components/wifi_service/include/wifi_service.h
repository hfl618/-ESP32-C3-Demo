#ifndef __WIFI_SERVICE_H__
#define __WIFI_SERVICE_H__

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "sys_msg.h"
#include <stdbool.h>

#define MAX_WIFI_PROFILES 3

typedef struct {
    char ssid[32];
    char password[64];
} wifi_profile_t;

typedef struct {
    uint8_t count;
    wifi_profile_t profiles[MAX_WIFI_PROFILES];
} wifi_db_t;

/* 初始化 WiFi 基础资源（不联网） */
esp_err_t wifi_service_init(void);

/* 开启 WiFi 联网（自动关蓝牙） */
void wifi_service_on(void);

/* 彻底关闭 WiFi 联网并释放射频 */
void wifi_service_off(void);

QueueHandle_t wifi_service_get_queue(void);
int wifi_service_get_rssi(void);
bool wifi_service_is_connected(void);
void wifi_service_scan(void);
void wifi_service_print_stored_configs(void);
esp_err_t wifi_service_save_db(wifi_db_t *db);
esp_err_t wifi_service_load_db(wifi_db_t *db);

#endif
