/**
 * @file storage_manager.h
 * @brief 通用存储管理器：负责 NVS 的持久化读写
 * 
 * 职责：提供整数(int32)和块数据(blob)的通用读写接口，屏蔽 NVS 句柄操作。
 */

#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include "esp_err.h"
#include <stdint.h>
#include <stddef.h>

/**
 * @brief 初始化 NVS 存储系统
 * 
 * 必须在系统启动初期调用。
 */
void storage_init(void);

/**
 * @brief 保存一个整数到存储器 (Key-Value 模式)
 * 
 * @param key   设置项的名称 (最大 15 字符)
 * @param value 要保存的数值
 */
void storage_set_int(const char* key, int32_t value);

/**
 * @brief 从存储器读取一个整数
 * 
 * @param key         设置项的名称
 * @param default_val 若项不存在时返回的默认值
 * @return int32_t    读取到的值或默认值
 */
int32_t storage_get_int(const char* key, int32_t default_val);

/**
 * @brief 保存一块二进制数据（如结构体）到存储器
 * 
 * @param key   设置项名称
 * @param data  数据指针
 * @param len   数据长度
 * @return esp_err_t ESP_OK 表示成功
 */
esp_err_t storage_set_blob(const char* key, const void* data, size_t len);

/**
 * @brief 从存储器读取一块二进制数据
 * 
 * @param key       设置项名称
 * @param out_data  用于存放读取结果的缓冲区指针
 * @param len       [in/out] 输入缓冲区大小，输出实际读取大小
 * @return esp_err_t ESP_OK 表示成功
 */
esp_err_t storage_get_blob(const char* key, void* out_data, size_t* len);

#endif /* STORAGE_MANAGER_H */
