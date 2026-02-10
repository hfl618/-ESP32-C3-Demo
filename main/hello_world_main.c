#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "st7735s.h"
#include "gfx.h"

void app_main(void)
{
    printf("ST7735S Clean Migration Start...\n");

    ST7735S_Init();
    setOrientation(R0); // 库默认就是横屏逻辑

    while (1) {
        printf("Loop: RED\n");
        setColor(255, 0, 0);
        fillScreen();
        vTaskDelay(pdMS_TO_TICKS(1000));

        printf("Loop: BLUE\n");
        setColor(0, 0, 255);
        fillScreen();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
