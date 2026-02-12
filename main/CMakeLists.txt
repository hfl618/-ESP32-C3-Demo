# 显式列出所有源文件，确保可靠构建
set(SOURCES
    "hello_world_main.c"
    "ui/ui_manager.c"
    "ui/components/ui_status_bar.c"
    "ui/pages/page_main.c"
    "ui/pages/page_settings.c"
    "ui/icon/settings.c"
    "ui/icon/wifi.c"
    "ui/icon/music.c"
    "ui/icon/message_ai.c"
)

# 定义包含路径
set(INCLUDE_DIRS 
    "." 
    "ui"
    "ui/components"
    "ui/pages"
    "ui/icon"
)

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS ${INCLUDE_DIRS}
                    REQUIRES st7735 lvgl ec11)
