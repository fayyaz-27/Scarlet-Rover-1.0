idf_component_register(SRCS "driver.c" "ir_driver.c" "main.c" "rx.c" "mcpwm_driver_2.c" "mcpwm_driver.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_wifi driver
                       PRIV_REQUIRES nvs_flash)
