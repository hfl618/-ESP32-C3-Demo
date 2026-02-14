#include "wifi_service.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include <string.h>
#include <time.h>

static const char *TAG = "WIFI_SVC";
static QueueHandle_t sys_event_queue = NULL;
static bool is_connecting = false;

static void send_sys_msg(msg_source_t src, int evt) {
    sys_msg_t msg = { .source = src, .event = evt, .data = NULL };
    if (sys_event_queue) xQueueSend(sys_event_queue, &msg, 0);
}

/* --- NTP 同步完成回调 --- */
void time_sync_notification_cb(struct timeval *tv) {
    ESP_LOGI(TAG, "Time Synchronized! Stopping SNTP service.");
    
    // 【关键】发送成功信号给 UI
    send_sys_msg(MSG_SOURCE_WIFI, WIFI_EVT_TIME_SYNCED);
    
    // 【核心要求】同步成功一次后立刻停止对时服务，不再重复对时
    esp_netif_sntp_deinit();
}

static void start_ntp_sync(void) {
    ESP_LOGI(TAG, "Starting NTP sync once...");
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("ntp.aliyun.com");
    config.sync_cb = time_sync_notification_cb;
    esp_netif_sntp_init(&config);
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        is_connecting = false;
        send_sys_msg(MSG_SOURCE_WIFI, WIFI_EVT_CONNECTED);
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        is_connecting = false;
        send_sys_msg(MSG_SOURCE_WIFI, WIFI_EVT_DISCONNECTED);
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        is_connecting = false;
        send_sys_msg(MSG_SOURCE_WIFI, WIFI_EVT_GOT_IP);
        start_ntp_sync(); // 获取 IP 后启动对时
    }
}

esp_err_t wifi_service_init(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    sys_event_queue = xQueueCreate(10, sizeof(sys_msg_t));
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_wifi_set_max_tx_power(34); 

    return ESP_OK;
}

void wifi_service_connect(void) {
    if (is_connecting) return;
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) return;

    wifi_config_t wifi_config = {
        .sta = { .ssid = "hhh", .password = "12345678", .threshold.authmode = WIFI_AUTH_WPA2_PSK },
    };
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    is_connecting = true;
    esp_wifi_connect();
}

QueueHandle_t wifi_service_get_queue(void) { return sys_event_queue; }
