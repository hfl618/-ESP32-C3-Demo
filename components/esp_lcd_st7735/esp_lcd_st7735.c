#include <stdlib.h>
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_st7735.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef struct {
    esp_lcd_panel_t base;
    esp_lcd_panel_io_handle_t io;
    int reset_gpio_num;
    bool reset_level;
    int x_gap;
    int y_gap;
    uint8_t fb_bits_per_pixel;
    uint8_t madctl_val;
} st7735_panel_t;

static esp_err_t panel_st7735_del(esp_lcd_panel_t *panel);
static esp_err_t panel_st7735_reset(esp_lcd_panel_t *panel);
static esp_err_t panel_st7735_init(esp_lcd_panel_t *panel);
static esp_err_t panel_st7735_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data);
static esp_err_t panel_st7735_invert_color(esp_lcd_panel_t *panel, bool invert_color_data);
static esp_err_t panel_st7735_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y);
static esp_err_t panel_st7735_swap_xy(esp_lcd_panel_t *panel, bool swap_xy);
static esp_err_t panel_st7735_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap);
static esp_err_t panel_st7735_disp_on_off(esp_lcd_panel_t *panel, bool off);

esp_err_t esp_lcd_new_panel_st7735(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel)
{
    st7735_panel_t *st7735 = calloc(1, sizeof(st7735_panel_t));
    st7735->io = io;
    st7735->reset_gpio_num = panel_dev_config->reset_gpio_num;
    st7735->reset_level = panel_dev_config->flags.reset_active_high;
    st7735->fb_bits_per_pixel = panel_dev_config->bits_per_pixel;
    st7735->base.del = panel_st7735_del;
    st7735->base.reset = panel_st7735_reset;
    st7735->base.init = panel_st7735_init;
    st7735->base.draw_bitmap = panel_st7735_draw_bitmap;
    st7735->base.invert_color = panel_st7735_invert_color;
    st7735->base.set_gap = panel_st7735_set_gap;
    st7735->base.mirror = panel_st7735_mirror;
    st7735->base.swap_xy = panel_st7735_swap_xy;
    st7735->base.disp_on_off = panel_st7735_disp_on_off;
    *ret_panel = &(st7735->base);
    return ESP_OK;
}

static esp_err_t panel_st7735_reset(esp_lcd_panel_t *panel)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);
    if (st7735->reset_gpio_num >= 0) {
        gpio_set_direction(st7735->reset_gpio_num, GPIO_MODE_OUTPUT);
        gpio_set_level(st7735->reset_gpio_num, st7735->reset_level);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(st7735->reset_gpio_num, !st7735->reset_level);
        vTaskDelay(pdMS_TO_TICKS(120));
    } else {
        esp_lcd_panel_io_tx_param(st7735->io, 0x01, NULL, 0); // SWRESET
        vTaskDelay(pdMS_TO_TICKS(120));
    }
    return ESP_OK;
}

static esp_err_t panel_st7735_init(esp_lcd_panel_t *panel)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7735->io;

    // ST7735 "Green Tab" Init Sequence (最常用)
    esp_lcd_panel_io_tx_param(io, 0x11, NULL, 0); // Sleep Out
    vTaskDelay(pdMS_TO_TICKS(120));
    
    esp_lcd_panel_io_tx_param(io, 0xB1, (uint8_t[]){0x01, 0x2C, 0x2D}, 3); 
    esp_lcd_panel_io_tx_param(io, 0xB2, (uint8_t[]){0x01, 0x2C, 0x2D}, 3); 
    esp_lcd_panel_io_tx_param(io, 0xB3, (uint8_t[]){0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D}, 6); 
    esp_lcd_panel_io_tx_param(io, 0xB4, (uint8_t[]){0x07}, 1); 

    esp_lcd_panel_io_tx_param(io, 0xC0, (uint8_t[]){0xA2, 0x02, 0x84}, 3); 
    esp_lcd_panel_io_tx_param(io, 0xC1, (uint8_t[]){0xC5}, 1); 
    esp_lcd_panel_io_tx_param(io, 0xC2, (uint8_t[]){0x0A, 0x00}, 2); 
    esp_lcd_panel_io_tx_param(io, 0xC3, (uint8_t[]){0x8A, 0x2A}, 2); 
    esp_lcd_panel_io_tx_param(io, 0xC4, (uint8_t[]){0x8A, 0xEE}, 2); 
    
    esp_lcd_panel_io_tx_param(io, 0xC5, (uint8_t[]){0x0E}, 1); 
    
    esp_lcd_panel_io_tx_param(io, 0x36, (uint8_t[]){0xC8}, 1); // MX, MY, RGB

    esp_lcd_panel_io_tx_param(io, 0xE0, (uint8_t[]){0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10}, 16);
    esp_lcd_panel_io_tx_param(io, 0xE1, (uint8_t[]){0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10}, 16);
    
    esp_lcd_panel_io_tx_param(io, 0x3A, (uint8_t[]){0x05}, 1); 
    esp_lcd_panel_io_tx_param(io, 0x29, NULL, 0); 
    vTaskDelay(pdMS_TO_TICKS(100));

    return ESP_OK;
}

static esp_err_t panel_st7735_draw_bitmap(esp_lcd_panel_t *panel, int x_start, int y_start, int x_end, int y_end, const void *color_data)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);
    esp_lcd_panel_io_handle_t io = st7735->io;
    x_start += st7735->x_gap; x_end += st7735->x_gap;
    y_start += st7735->y_gap; y_end += st7735->y_gap;
    esp_lcd_panel_io_tx_param(io, 0x2A, (uint8_t[]){(x_start >> 8) & 0xFF, x_start & 0xFF, ((x_end - 1) >> 8) & 0xFF, (x_end - 1) & 0xFF}, 4);
    esp_lcd_panel_io_tx_param(io, 0x2B, (uint8_t[]){(y_start >> 8) & 0xFF, y_start & 0xFF, ((y_end - 1) >> 8) & 0xFF, (y_end - 1) & 0xFF}, 4);
    size_t len = (x_end - x_start) * (y_end - y_start) * st7735->fb_bits_per_pixel / 8;
    esp_lcd_panel_io_tx_color(io, 0x2C, color_data, len);
    return ESP_OK;
}

static esp_err_t panel_st7735_invert_color(esp_lcd_panel_t *panel, bool invert_color_data)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);
    esp_lcd_panel_io_tx_param(st7735->io, invert_color_data ? 0x21 : 0x20, NULL, 0);
    return ESP_OK;
}

static esp_err_t panel_st7735_mirror(esp_lcd_panel_t *panel, bool mirror_x, bool mirror_y)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);
    if (mirror_x) st7735->madctl_val |= (1 << 6); else st7735->madctl_val &= ~(1 << 6);
    if (mirror_y) st7735->madctl_val |= (1 << 7); else st7735->madctl_val &= ~(1 << 7);
    esp_lcd_panel_io_tx_param(st7735->io, 0x36, (uint8_t[]){st7735->madctl_val}, 1);
    return ESP_OK;
}

static esp_err_t panel_st7735_swap_xy(esp_lcd_panel_t *panel, bool swap_xy)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);
    if (swap_xy) st7735->madctl_val |= (1 << 5); else st7735->madctl_val &= ~(1 << 5);
    esp_lcd_panel_io_tx_param(st7735->io, 0x36, (uint8_t[]){st7735->madctl_val}, 1);
    return ESP_OK;
}

static esp_err_t panel_st7735_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);
    st7735->x_gap = x_gap; st7735->y_gap = y_gap;
    return ESP_OK;
}

static esp_err_t panel_st7735_disp_on_off(esp_lcd_panel_t *panel, bool off)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);
    esp_lcd_panel_io_tx_param(st7735->io, off ? 0x28 : 0x29, NULL, 0);
    return ESP_OK;
}

static esp_err_t panel_st7735_del(esp_lcd_panel_t *panel)
{
    st7735_panel_t *st7735 = __containerof(panel, st7735_panel_t, base);
    free(st7735);
    return ESP_OK;
}
