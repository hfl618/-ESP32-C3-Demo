# 仅包含入口源文件
set(SOURCES "hello_world_main.c")

# 定义包含路径
set(INCLUDE_DIRS ".")

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS ${INCLUDE_DIRS}
                    REQUIRES st7735 lvgl ec11 bsp ui uart_service)
