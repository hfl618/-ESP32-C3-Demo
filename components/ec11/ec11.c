#include "ec11.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "bsp_board.h"

volatile int pcnt_count = 0;
volatile uint32_t isr_cnt = 0;
static int last_pcnt_count = 0;
static bool last_sw_level = 1;

static void IRAM_ATTR ec11_isr_handler(void* arg)
{
    isr_cnt++; 
    static uint8_t old_state = 0;
    uint8_t a = gpio_get_level(BSP_EC11_A_PIN);
    uint8_t b = gpio_get_level(BSP_EC11_B_PIN);
    uint8_t new_state = (a << 1) | b;

    static const int8_t states[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
    pcnt_count += states[(old_state << 2) | new_state];
    old_state = new_state;
}

esp_err_t ec11_init(void)
{
    // A/B 相初始化保持不变
    gpio_reset_pin(BSP_EC11_A_PIN);
    gpio_reset_pin(BSP_EC11_B_PIN);
    gpio_set_direction(BSP_EC11_A_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(BSP_EC11_B_PIN, GPIO_MODE_INPUT);
    gpio_pullup_en(BSP_EC11_A_PIN);
    gpio_pullup_en(BSP_EC11_B_PIN);

    // 强化 SW 引脚初始化
    gpio_reset_pin(BSP_EC11_SW_PIN);
    gpio_config_t sw_conf = {
        .pin_bit_mask = (1ULL << BSP_EC11_SW_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&sw_conf);

    gpio_set_intr_type(BSP_EC11_A_PIN, GPIO_INTR_ANYEDGE);
    gpio_set_intr_type(BSP_EC11_B_PIN, GPIO_INTR_ANYEDGE);

    esp_err_t err = gpio_install_isr_service(0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) return err;

    gpio_isr_handler_add(BSP_EC11_A_PIN, ec11_isr_handler, (void*)BSP_EC11_A_PIN);
    gpio_isr_handler_add(BSP_EC11_B_PIN, ec11_isr_handler, (void*)BSP_EC11_B_PIN);

    // 同步初始计数值，防止开机时 delta 过大导致焦点乱跳
    last_pcnt_count = pcnt_count / 4;

    return ESP_OK;
}

int ec11_get_delta(void)
{
    int pcnt_snapshot = pcnt_count; 
    int cur_count = pcnt_snapshot / 4; 
    int delta = cur_count - last_pcnt_count;
    if (delta != 0) {
        last_pcnt_count = cur_count;
    }
    return delta;
}

bool ec11_is_pressed(void) { 
    return gpio_get_level(BSP_EC11_SW_PIN) == 0; 
}

bool ec11_check_click(void)
{
    bool curr_sw = gpio_get_level(BSP_EC11_SW_PIN);
    bool clicked = (last_sw_level == 0 && curr_sw == 1);
    last_sw_level = curr_sw;
    return clicked;
}
