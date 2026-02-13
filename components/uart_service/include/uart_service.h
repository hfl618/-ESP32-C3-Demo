#ifndef __UART_SERVICE_H__
#define __UART_SERVICE_H__

#include "esp_err.h"

#define UART_BUF_SIZE 1024

/**
 * @brief 初始化串口服务 (UART1)
 */
esp_err_t uart_service_init(void);

/**
 * @brief 发送字符串到串口
 */
void uart_send_string(const char* data);

#endif
