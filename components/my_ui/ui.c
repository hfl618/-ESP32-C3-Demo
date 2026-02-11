#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "st7735s.h"
#include "gfx.h"
#include "fonts.h"
#include "ui.h"
#include "ec11.h"

typedef enum {
    PAGE_HOME,
    PAGE_MENU,
    PAGE_SET_BRIGHTNESS,
    PAGE_ABOUT
} page_t;

static void gui_demo_task(void *pvParameters)
{
    ST7735S_Init();
    ec11_init();
    setTransparent(true);

    page_t current_page = PAGE_HOME;
    page_t last_page = -1; 

    int counter = 0;
    int brightness = 50;
    int menu_index = 0; 
    char buf[32];

    while (1) {
        int delta = ec11_get_delta();
        bool clicked = ec11_check_click();
        bool force_redraw = (current_page != last_page);

        if (current_page == PAGE_HOME) {
            counter += delta;
            if (clicked) current_page = PAGE_MENU;
        } 
        else if (current_page == PAGE_MENU) {
            menu_index += delta;
            if (menu_index < 0) menu_index = 1; 
            if (menu_index > 1) menu_index = 0;
            if (clicked) {
                if (menu_index == 0) current_page = PAGE_SET_BRIGHTNESS;
                else current_page = PAGE_ABOUT;
            }
        }
        else if (current_page == PAGE_SET_BRIGHTNESS) {
            brightness += delta * 5;
            if (brightness < 0) brightness = 0;
            if (brightness > 100) brightness = 100;
            Backlight_Pct(brightness); 
            if (clicked) current_page = PAGE_MENU;
        }
        else if (current_page == PAGE_ABOUT) {
            if (clicked) current_page = PAGE_MENU;
        }

        // --- 渲染处理 (优化版) ---
        if (delta != 0 || clicked || force_redraw) {
            
            if (force_redraw) {
                setColor(20, 20, 60); 
                fillScreen();
                last_page = current_page;
            }

            setColor(0, 255, 150); 
            setFont(ter_u16b);
            if (current_page == PAGE_HOME) drawText(10, 5, "DASHBOARD");
            else if (current_page == PAGE_MENU) drawText(10, 5, "MAIN MENU");
            else if (current_page == PAGE_SET_BRIGHTNESS) drawText(10, 5, "BRIGHTNESS");
            else drawText(10, 5, "ABOUT");

            setColor(100, 100, 100);
            drawLine(0, 26, 159, 26);

            // 【关键点】在绘制列表前，先整体擦除一次内容区域，防止残影
            setColor(20, 20, 60); // 使用背景色
            filledRect(5, 30, 155, 75);

            if (current_page == PAGE_HOME) {
                setColor(40, 40, 100); filledRect(10, 32, 150, 48); // 数值槽
                setColor(255, 255, 255); setFont(ter_u12b);
                sprintf(buf, "Total Steps: %d", counter);
                drawText(15, 35, buf);
                drawText(15, 55, "Click to Menu >");
            } 
            else if (current_page == PAGE_MENU) {
                setFont(ter_u12b);
                // 绘制选项 1 (只在选中的时候画出 >)
                if (menu_index == 0) {
                    setColor(255, 255, 0); drawText(10, 35, "> Set Brightness");
                } else {
                    setColor(200, 200, 200); drawText(10, 35, "  Set Brightness");
                }
                // 绘制选项 2
                if (menu_index == 1) {
                    setColor(255, 255, 0); drawText(10, 55, "> About System");
                } else {
                    setColor(200, 200, 200); drawText(10, 55, "  About System");
                }
            }
            else if (current_page == PAGE_SET_BRIGHTNESS) {
                setColor(40, 40, 100); filledRect(30, 40, 130, 60);
                setColor(255, 255, 0); setFont(ter_u12b);
                sprintf(buf, "Level: %d%%", brightness);
                drawText(45, 45, buf);
            }
            else if (current_page == PAGE_ABOUT) {
                setColor(255, 255, 255); setFont(ter_u12b);
                drawText(15, 35, "ESP32-C3 v1.0");
                drawText(15, 55, "ST7735S Driver");
            }

            flushBuffer();
        }
        vTaskDelay(pdMS_TO_TICKS(30)); 
    }
}

void start_ui_demo(void)
{
    xTaskCreate(gui_demo_task, "gui_demo", 8192, NULL, 5, NULL);
}