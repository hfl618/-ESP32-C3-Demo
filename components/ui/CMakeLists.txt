file(GLOB_RECURSE SOURCES 
    "ui_manager.c"
    "components/*.c"
    "pages/*.c"
    "icon/*.c"
)

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS "." "components" "pages" "icon"
                    REQUIRES st7735 lvgl ec11)
