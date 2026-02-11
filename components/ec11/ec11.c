#include "ec11.h"
#include "driver/gpio.h"
#include "esp_log.h"

static volatile int raw_count = 0;
static int last_stable_count = 0;
static bool last_sw_level = 1;

static void IRAM_ATTR ec11_gpio_isr_handler(void* arg)
{
    // 旋转逻辑：使用更稳定的状态判断
    static uint8_t ab_state = 0;
    ab_state = (ab_state << 2) | (gpio_get_level(EC11_GPIO_A) << 1) | gpio_get_level(EC11_GPIO_B);
    
    // 典型的正交编码器状态表 (针对常见的点击感旋钮)
    // 只有当旋转完成一个完整步进周期时才计数
    switch (ab_state & 0x0F) {
        case 0b0010: case 0b1011: case 0b1101: case 0b0100: raw_count--; break;
        case 0b0001: case 0b0111: case 0b1110: case 0b1000: raw_count++; break;
    }
}

esp_err_t ec11_init(void)
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << EC11_GPIO_A) | (1ULL << EC11_GPIO_B),
        .pull_up_en = 1,
    };
    gpio_config(&io_conf);

    gpio_config_t sw_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << EC11_GPIO_SW),
        .pull_up_en = 1,
    };
    gpio_config(&sw_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(EC11_GPIO_A, ec11_gpio_isr_handler, (void*) EC11_GPIO_A);
    gpio_isr_handler_add(EC11_GPIO_B, ec11_gpio_isr_handler, (void*) EC11_GPIO_B);

    return ESP_OK;
}

// 获取自上次调用以来的“变化量”（Delta）
// 老师提示：这是实现多页面交互的秘诀！
int ec11_get_delta(void)
{
    int current = raw_count / 4; // 大多数旋钮物理上一格对应4个状态变化
    int delta = current - last_stable_count;
    last_stable_count = current;
    return delta;
}

// 核心逻辑：检测单击（按下并弹起）
bool ec11_check_click(void)
{
    bool curr_sw = gpio_get_level(EC11_GPIO_SW);
    bool clicked = false;

    if (last_sw_level == 0 && curr_sw == 1) { // 检测到上升沿（释放按键的一瞬间）
        clicked = true;
    }
    last_sw_level = curr_sw;
    return clicked;
}

bool ec11_is_pressed(void)
{
    return gpio_get_level(EC11_GPIO_SW) == 0;
}