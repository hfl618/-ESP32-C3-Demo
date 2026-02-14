# 自动搜索所有子目录下的 .c 文件
file(GLOB_RECURSE SOURCES "*.c")

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS "."
                    REQUIRES lvgl common wifi_service)
