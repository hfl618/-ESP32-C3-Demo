#include "st7735s_compat.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

#define PIN_NUM_MOSI 6
#define PIN_NUM_CLK  4
#define PIN_NUM_DC   3
#define PIN_NUM_RST  10

static spi_device_handle_t spi;

void SPI_Init(void) {
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
        .spics_io_num = -1, // CS接GND
        .queue_size = 7,
    };
    spi_bus_add_device(SPI2_HOST, &devcfg, &spi);

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_NUM_DC) | (1ULL << PIN_NUM_RST),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf);
}

void Pin_CS_Low(void) {}
void Pin_CS_High(void) {}
void Pin_RES_High(void) { gpio_set_level(PIN_NUM_RST, 1); }
void Pin_RES_Low(void)  { gpio_set_level(PIN_NUM_RST, 0); }
void Pin_DC_High(void)  { gpio_set_level(PIN_NUM_DC, 1); }
void Pin_DC_Low(void)   { gpio_set_level(PIN_NUM_DC, 0); }
void Pin_BLK_Pct(uint8_t pct) {}

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