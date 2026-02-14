idf_component_register(SRCS "gui_engine.c"
                    INCLUDE_DIRS "include"
                    REQUIRES st7735 lvgl ec11 ui wifi_service common)
