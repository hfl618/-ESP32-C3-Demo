# 仅包含入口源文件
set(SOURCES "hello_world_main.c")

# 定义包含路径
set(INCLUDE_DIRS ".")

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS ${INCLUDE_DIRS}
                    REQUIRES bsp uart_service wifi_service gui_engine ble_service)
