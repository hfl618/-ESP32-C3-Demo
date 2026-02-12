#include "ec11.h"
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "esp_log.h"

static pcnt_unit_handle_t pcnt_unit = NULL;
static int last_pcnt_count = 0;
static bool last_sw_level = 1;

esp_err_t ec11_init(void)
{
    // 1. 配置 PCNT 单元
    pcnt_unit_config_t unit_config = {
        .high_limit = 1000,
        .low_limit = -1000,
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    // 2. 配置硬件滤波器 (关键：滤除 10us 以内的机械噪声)
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 10000, // 10000ns = 10us
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    // 3. 配置 A/B 通道 (正交解码模式)
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = EC11_GPIO_A,
        .level_gpio_num = EC11_GPIO_B,
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));

    pcnt_chan_config_t chan_b_config = {
        .edge_gpio_num = EC11_GPIO_B,
        .level_gpio_num = EC11_GPIO_A,
    };
    pcnt_channel_handle_t pcnt_chan_b = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_b_config, &pcnt_chan_b));

    // 4. 设置正交编码器动作 (四倍频计数)
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    // 5. 启动 PCNT
    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));

    // 6. 配置按键 GPIO
    gpio_config_t sw_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << EC11_GPIO_SW),
        .pull_up_en = 1,
    };
    gpio_config(&sw_conf);

    return ESP_OK;
}

int ec11_get_delta(void)
{
    int cur_count = 0;
    pcnt_unit_get_count(pcnt_unit, &cur_count);
    
    // EC11 通常转一格对应 4 个相位变化
    int current_clicks = cur_count / 4;
    int delta = current_clicks - last_pcnt_count;
    
    // 如果没有变化，不更新 last_pcnt_count，防止微小抖动被吞掉
    if (delta != 0) {
        last_pcnt_count = current_clicks;
    }
    
    return delta;
}

bool ec11_check_click(void)
{
    bool curr_sw = gpio_get_level(EC11_GPIO_SW);
    bool clicked = (last_sw_level == 0 && curr_sw == 1);
    last_sw_level = curr_sw;
    return clicked;
}

bool ec11_is_pressed(void)
{
    return gpio_get_level(EC11_GPIO_SW) == 0;
}
