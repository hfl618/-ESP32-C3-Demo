idf_component_register(SRCS "hello_world_main.c"
                    INCLUDE_DIRS "."
                    REQUIRES my_ui ec11 st7735 esp_driver_gpio idf::esp_driver_ledc)