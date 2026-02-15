/**
 * @file storage_manager.c
 * @brief 存储管理器实现：封装 ESP-IDF 的 NVS (Non-volatile Storage) API
 * 
 * 优势：
 * 1. 自动处理 NVS 初始化及异常修复（如 Flash 分区截断）。
 * 2. 统一管理存储命名空间，避免不同组件间的 Key 冲突。
 * 3. 简化读写流程，调用方无需手动处理 nvs_open/nvs_close/nvs_commit。
 */

#include "storage_manager.h"
#include "nvs_flash.h"
#include "esp_log.h"

static const char *TAG = "STORAGE_MGR";
static const char *NAMESPACE = "storage"; // 统一使用原有存储命名空间

/**
 * @brief 初始化 NVS 系统
 */
void storage_init(void) {
    // 尝试初始化默认 NVS 分区
    esp_err_t ret = nvs_flash_init();
    
    // 处理 NVS 空间不足或版本不兼容的情况
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS state abnormal, erasing and resetting...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "NVS service ready (Namespace: %s)", NAMESPACE);
}

/**
 * @brief 保存一个整数值
 */
void storage_set_int(const char* key, int32_t value) {
    nvs_handle_t handle;
    // 以读写模式打开命名空间
    if (nvs_open(NAMESPACE, NVS_READWRITE, &handle) == ESP_OK) {
        // 设置数值
        nvs_set_i32(handle, key, value);
        // 提交到 Flash（这一步是真正的物理写入）
        nvs_commit(handle);
        // 释放句柄
        nvs_close(handle);
        ESP_LOGI(TAG, "Save Int: %s = %ld", key, (long)value);
    } else {
        ESP_LOGE(TAG, "Open NVS failed, cannot save %s", key);
    }
}

/**
 * @brief 读取一个整数值
 */
int32_t storage_get_int(const char* key, int32_t default_val) {
    nvs_handle_t handle;
    int32_t value = default_val;
    // 以只读模式打开
    if (nvs_open(NAMESPACE, NVS_READONLY, &handle) == ESP_OK) {
        // 尝试读取，若 Key 不存在，value 保持 default_val
        nvs_get_i32(handle, key, &value);
        nvs_close(handle);
    }
    return value;
}

/**
 * @brief 保存二进制块数据（适用于结构体）
 */
esp_err_t storage_set_blob(const char* key, const void* data, size_t len) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;

    // 写入 Blob 数据
    err = nvs_set_blob(handle, key, data, len);
    if (err == ESP_OK) {
        nvs_commit(handle); // 物理写入
    }
    
    nvs_close(handle);
    ESP_LOGI(TAG, "Save Blob: %s, Len: %u", key, (unsigned int)len);
    return err;
}

/**
 * @brief 读取二进制块数据
 */
esp_err_t storage_get_blob(const char* key, void* out_data, size_t* len) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) return err;

    // 读取 Blob 数据，len 会被更新为实际读取的长度
    err = nvs_get_blob(handle, key, out_data, len);
    nvs_close(handle);
    return err;
}
