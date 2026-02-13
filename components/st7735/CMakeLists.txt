idf_component_register(SRCS "st7735s.c" "gfx.c" "fonts.c" "st7735s_compat.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver
                    PRIV_REQUIRES bsp)
