#include "st7735s_compat.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/ledc.h" // 【新增】引入 LED 控制器头文件
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

#define PIN_NUM_MOSI 6
#define PIN_NUM_CLK  4
#define PIN_NUM_DC   3
#define PIN_NUM_RST  10
#define PIN_NUM_BCKL 2  // 【新增】定义背光引脚为 GPIO 2

static spi_device_handle_t spi;

void SPI_Init(void) {
    // 1. 初始化 SPI 总线 (保持不变)
    spi_bus_config_t buscfg = {
        .miso_io_num = -1,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = defWIDTH * defHEIGHT * 2
    };
    spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 5 * 1000 * 1000, 
        .mode = 0,
        .spics_io_num = -1, 
        .queue_size = 7,
    };
    spi_bus_add_device(SPI2_HOST, &devcfg, &spi);

    // 2. 初始化 GPIO (DC, RST)
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_NUM_DC) | (1ULL << PIN_NUM_RST),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf);

    // 3. 【核心】配置 PWM 背光控制
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_8_BIT, // 8位分辨率 (0-255)
        .freq_hz          = 5000,             // 5kHz 频率，人眼看不见闪烁
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = PIN_NUM_BCKL,
        .duty           = 255, // 初始全亮
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
}

// 修改背光百分比的函数
void Pin_BLK_Pct(uint8_t pct) {
    if (pct > 100) pct = 100;
    // 将 0-100% 转换为 0-255 的占空比
    uint32_t duty = (pct * 255) / 100;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void Pin_CS_Low(void) {}
void Pin_CS_High(void) {}
void Pin_RES_High(void) { gpio_set_level(PIN_NUM_RST, 1); }
void Pin_RES_Low(void)  { gpio_set_level(PIN_NUM_RST, 0); }
void Pin_DC_High(void)  { gpio_set_level(PIN_NUM_DC, 1); }
void Pin_DC_Low(void)   { gpio_set_level(PIN_NUM_DC, 0); }

void SPI_TransmitCmd(uint16_t len, uint8_t *data) {
    Pin_DC_Low();
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;
    spi_device_polling_transmit(spi, &t);
}

void SPI_TransmitData(uint16_t len, uint8_t *data) {
    Pin_DC_High();
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;
    spi_device_polling_transmit(spi, &t);
}

void SPI_Transmit(uint16_t len, uint8_t *data) {
    SPI_TransmitCmd(1, data++);
    if (--len) SPI_TransmitData(len, data);
}

void _Delay(uint32_t d) {
    vTaskDelay(pdMS_TO_TICKS(d));
}
