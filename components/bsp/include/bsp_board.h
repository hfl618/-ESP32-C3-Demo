#ifndef __BSP_BOARD_H__
#define __BSP_BOARD_H__

#include "driver/gpio.h"
#include "esp_err.h"

/* --- 串口引脚 (UART) --- */
#define BSP_UART_TX_PIN   21
#define BSP_UART_RX_PIN   20

/* --- 编码器引脚 (EC11) --- */
#define BSP_EC11_A_PIN    0
#define BSP_EC11_B_PIN    1
#define BSP_EC11_SW_PIN   5

/* --- 屏幕引脚 (ST7735 SPI) --- */
#define BSP_LCD_SDA_PIN   6
#define BSP_LCD_SCL_PIN   4
#define BSP_LCD_DC_PIN    3
#define BSP_LCD_RES_PIN   10
#define BSP_LCD_BLK_PIN   2

esp_err_t bsp_board_init(void);

#endif
