#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ui.h" // 只需要包含我们的 UI 接口

void app_main(void)
{
    printf("System Booting...\n");
    
    // 启动 UI 模块
    start_ui_demo();

    // app_main 可以去做其他核心逻辑，比如处理网络、传感器等
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}