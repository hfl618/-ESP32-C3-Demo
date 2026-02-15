#include "ble_service.h"
#include "esp_log.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_att.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "cJSON.h"
#include "wifi_service.h"
#include <string.h>
#include "esp_bt.h"

static const char *TAG = "BLE_SVC";
static const char *device_name = "ESP32-C3-Config";
static uint8_t ble_addr_type;
static TaskHandle_t ble_host_task_handle = NULL;
static bool ble_initialized = false;

static const ble_uuid128_t svc_uuid = BLE_UUID128_INIT(0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12);
static const ble_uuid128_t char_uuid = BLE_UUID128_INIT(0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12, 0x01, 0xef, 0xcd, 0xab);

static int ble_svc_gatt_handler(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);
static int ble_gap_event(struct ble_gap_event *event, void *arg);

static const struct ble_gatt_svc_def ble_svc_defs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid = &char_uuid.u,
                .access_cb = ble_svc_gatt_handler,
                .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_READ,
            },
            {0}
        },
    },
    {0}
};

static void ble_app_advertise(void) {
    struct ble_hs_adv_fields fields = {0};
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);

    struct ble_gap_adv_params adv_params = {0};
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    
    // 广播间隔优化：100ms (1单位=0.625ms, 160*0.625=100)
    // 100ms 是连接稳定性与功耗的最佳平衡点
    adv_params.itvl_min = 160; 
    adv_params.itvl_max = 160;

    int rc = ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
    if (rc == 0) {
        ESP_LOGI(TAG, "Advertising (100ms interval)...");
        sys_msg_send(MSG_SOURCE_BT, BT_EVT_ADVERTISING, NULL);
    }
}

static void ble_on_sync(void) {
    ble_hs_id_infer_auto(0, &ble_addr_type);
    
    // 调整功率：使用 P6 (+6dBm)，接近 WiFi 的限制，提供更好的连接稳定性
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_P6);
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P6);
    
    ble_app_advertise();
}

static void ble_host_task(void *param) {
    nimble_port_run();
    nimble_port_freertos_deinit();
}

static int ble_gap_event(struct ble_gap_event *event, void *arg) {
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status == 0) {
                ESP_LOGI(TAG, "GATT Connected!");
                sys_msg_send(MSG_SOURCE_BT, BT_EVT_CONNECTED, NULL);
            } else {
                ESP_LOGE(TAG, "Connect failed; status=%d", event->connect.status);
                ble_app_advertise();
            }
            break;
        case BLE_GAP_EVENT_DISCONNECT:
            ESP_LOGI(TAG, "GATT Disconnected");
            sys_msg_send(MSG_SOURCE_BT, BT_EVT_DISCONNECTED, NULL);
            ble_app_advertise();
            break;
        case BLE_GAP_EVENT_MTU:
            ESP_LOGI(TAG, "MTU size: %d", event->mtu.value);
            break;
    }
    return 0;
}

void ble_service_on(void) {
    if (ble_host_task_handle) {
        ble_app_advertise();
        return;
    }
    
    wifi_service_off();
    vTaskDelay(pdMS_TO_TICKS(300)); 
    
    if (!ble_initialized) {
        nimble_port_init();
        ble_hs_cfg.sync_cb = ble_on_sync;
        ble_att_set_preferred_mtu(256);
        ble_svc_gap_init();
        ble_svc_gatt_init();
        ble_svc_gap_device_name_set(device_name);
        ble_gatts_count_cfg(ble_svc_defs);
        ble_gatts_add_svcs(ble_svc_defs);
        ble_initialized = true;
    }
    
    // 提升堆栈和优先级，确保连接过程不被干扰
    xTaskCreate(ble_host_task, "ble_host", 4096, NULL, 5, &ble_host_task_handle);
}

void ble_service_off(void) {
    if (!ble_host_task_handle) return;
    ble_gap_adv_stop();
    nimble_port_stop();
    vTaskDelete(ble_host_task_handle);
    ble_host_task_handle = NULL;
    ble_initialized = false;
    nimble_port_deinit();
}

static int ble_svc_gatt_handler(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
        char *data = malloc(ctxt->om->om_len + 1);
        ble_hs_mbuf_to_flat(ctxt->om, data, ctxt->om->om_len, NULL);
        data[ctxt->om->om_len] = '\0';
        cJSON *root = cJSON_Parse(data);
        if (root) {
            cJSON *list = cJSON_GetObjectItem(root, "list");
            if (list) {
                wifi_db_t db = {0};
                db.count = cJSON_GetArraySize(list);
                if (db.count > MAX_WIFI_PROFILES) db.count = MAX_WIFI_PROFILES;
                for (int i = 0; i < db.count; i++) {
                    cJSON *item = cJSON_GetArrayItem(list, i);
                    cJSON *s = cJSON_GetObjectItem(item, "s"), *p = cJSON_GetObjectItem(item, "p");
                    if (s) strncpy(db.profiles[i].ssid, s->valuestring, 32);
                    if (p) strncpy(db.profiles[i].password, p->valuestring, 64);
                }
                wifi_service_save_db(&db);
                esp_restart();
            }
            cJSON_Delete(root);
        }
        free(data);
    }
    return 0;
}

esp_err_t ble_service_init(void) { return ESP_OK; }
