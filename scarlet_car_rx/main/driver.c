// Project Name: SW420 Vibration Sensor Module: Driver Code
// Author: Fayyaz Nisar Shaikh
// LinkedIn: https://www.linkedin.com/in/fayyaz-shaikh-7646312a3/
// Email: fayyaz.shaikh7862005@gmail.com
// Phone: +91 8591686239



#include "stdio.h"

#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver.h"

void sw420_init(uint8_t gpio_number)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << gpio_number,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

int read_vibration(uint8_t gpio_number)
{

    return gpio_get_level(gpio_number);
}