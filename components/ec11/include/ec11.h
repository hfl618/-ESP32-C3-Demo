#ifndef __EC11_H__
#define __EC11_H__

#include <stdbool.h>
#include "esp_err.h"

#define EC11_GPIO_A   0
#define EC11_GPIO_B   1
#define EC11_GPIO_SW  5

esp_err_t ec11_init(void);
int ec11_get_delta(void);    // 获取变化量 (如 +1, -1)
bool ec11_check_click(void); // 检查是否发生了一次单击
bool ec11_is_pressed(void);

#endif
