#include "wifi_service.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include "ble_service.h"
#include <string.h>

#define WIFI_DB_NVS_KEY "wifi_db"
#define WIFI_DB_NAMESPACE "storage"

static const char *TAG = "WIFI_SVC";
static bool wifi_started = false;
static bool is_connected = false;
static wifi_db_t g_wifi_db = {0};
static int retry_cnt = 0;

static void send_sys_msg(msg_source_t src, int evt) {
    sys_msg_send(src, evt, NULL);
}

/* --- [调试] 以 HEX 格式打印字符串，用于检查隐藏字符 --- */
static void debug_print_hex(const char* label, const char* str) {
    printf("%s: ", label);
    for(int i=0; i<strlen(str); i++) printf("%02X ", str[i]);
    printf("(len: %d)\n", (int)strlen(str));
}

void wifi_service_print_stored_configs(void) {
    wifi_db_t temp_db = {0};
    nvs_handle_t handle;
    if (nvs_open(WIFI_DB_NAMESPACE, NVS_READONLY, &handle) == ESP_OK) {
        size_t size = sizeof(wifi_db_t);
        if (nvs_get_blob(handle, WIFI_DB_NVS_KEY, &temp_db, &size) == ESP_OK) {
            ESP_LOGI(TAG, "======= Stored WiFi Database =======");
            for (int i = 0; i < temp_db.count; i++) {
                ESP_LOGI(TAG, "[%d] SSID: %s", i, temp_db.profiles[i].ssid);
            }
            ESP_LOGI(TAG, "====================================");
        }
        nvs_close(handle);
    }
}

void wifi_service_on(void) {
    ble_service_off(); 

    if (!wifi_started) {
        ESP_LOGI(TAG, "Starting WiFi RF...");
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_start());
        esp_wifi_set_ps(WIFI_PS_NONE);
        wifi_started = true;
    }

    if (g_wifi_db.count > 0) {
        wifi_config_t wifi_config = {
            .sta = {
                .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            },
        };
        strncpy((char*)wifi_config.sta.ssid, g_wifi_db.profiles[0].ssid, 32);
        strncpy((char*)wifi_config.sta.password, g_wifi_db.profiles[0].password, 64);

        ESP_LOGI(TAG, "Auto-connecting to %s...", g_wifi_db.profiles[0].ssid);
        esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
        esp_wifi_set_max_tx_power(34); 
        esp_wifi_connect();
    } else {
        ESP_LOGW(TAG, "WiFi Power On, but no profiles found in DB!");
    }
}

void wifi_service_off(void) {
    if (!wifi_started) return;
    esp_wifi_stop();
    wifi_started = false;
    is_connected = false;
}

void wifi_service_connect_to(const char *ssid, const char *password) {
    ble_service_off(); 

    if (!wifi_started) {
        ESP_ERROR_CHECK(esp_wifi_start());
        esp_wifi_set_ps(WIFI_PS_NONE);
        wifi_started = true;
    }

    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    strncpy((char*)wifi_config.sta.ssid, ssid, 32);
    strncpy((char*)wifi_config.sta.password, password, 64);

    ESP_LOGI(TAG, "Connecting to %s...", ssid);
    esp_wifi_disconnect(); 
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_set_max_tx_power(34); 
    esp_wifi_connect();
}

int wifi_service_get_rssi(void) {
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) return ap_info.rssi;
    return -127;
}

bool wifi_service_is_connected(void) { return is_connected; }

esp_err_t wifi_service_save_db(wifi_db_t *db) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(WIFI_DB_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;
    err = nvs_set_blob(handle, WIFI_DB_NVS_KEY, db, sizeof(wifi_db_t));
    if (err == ESP_OK) nvs_commit(handle);
    nvs_close(handle);
    memcpy(&g_wifi_db, db, sizeof(wifi_db_t));
    return err;
}

esp_err_t wifi_service_load_db(wifi_db_t *db) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(WIFI_DB_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) return err;
    size_t size = sizeof(wifi_db_t);
    err = nvs_get_blob(handle, WIFI_DB_NVS_KEY, db, &size);
    nvs_close(handle);
    return err;
}

static void time_sync_notification_cb(struct timeval *tv) {
    ESP_LOGI(TAG, "Time Synced!");
    send_sys_msg(MSG_SOURCE_WIFI, WIFI_EVT_TIME_SYNCED);
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_set_country_code("CN", true);
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        wifi_event_sta_disconnected_t* evt = (wifi_event_sta_disconnected_t*) event_data;
        ESP_LOGW(TAG, "Link Down. Reason:%d", evt->reason);
        is_connected = false;
        
        if (wifi_started && retry_cnt < 5) {
            retry_cnt++;
            esp_wifi_connect();
        } else if (retry_cnt >= 5) {
            send_sys_msg(MSG_SOURCE_WIFI, WIFI_EVT_DISCONNECTED);
            retry_cnt = 0;
        }
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        is_connected = true; retry_cnt = 0;
        send_sys_msg(MSG_SOURCE_WIFI, WIFI_EVT_GOT_IP);
        
        time_t now;
        struct tm timeinfo;
        time(&now);
        localtime_r(&now, &timeinfo);

        if (timeinfo.tm_year > (2020 - 1900)) {
            ESP_LOGI(TAG, "Time already valid, skipping SNTP sync.");
        } else {
            static bool sntp_inited = false;
            if (!sntp_inited) {
                ESP_LOGI(TAG, "Initializing SNTP for the first time...");
                esp_sntp_config_t sntp_config = ESP_NETIF_SNTP_DEFAULT_CONFIG("ntp.aliyun.com");
                sntp_config.sync_cb = time_sync_notification_cb;
                esp_netif_sntp_init(&sntp_config);
                sntp_inited = true;
            }
        }
    }
}

esp_err_t wifi_service_init(void) {
    wifi_service_load_db(&g_wifi_db);
    wifi_service_print_stored_configs();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));
    return ESP_OK;
}

QueueHandle_t wifi_service_get_queue(void) { return g_sys_msg_queue; }
