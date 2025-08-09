// Project Name: MCPWM Motor 1 Driver Header Code
// Author: Fayyaz Nisar Shaikh
// LinkedIn: https://www.linkedin.com/in/fayyaz-shaikh-7646312a3/
// Email: fayyaz.shaikh7862005@gmail.com
// Phone: +91 8591686239



#ifndef MCPWM_DRIVER_H
#define MCPWM_DRIVER_H

// #pragma once

#include <stdio.h>

#include "esp_log.h"
#include "esp_system.h"
// #include "driver/mcpwm.h"
#include "driver/mcpwm_prelude.h"  


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "soc/mcpwm_periph.h"
#include "driver/gpio.h"

#include "mcpwm_driver.h"


esp_err_t mcpwm_driver_init(uint8_t fwd_gpio_num, uint8_t rev_gpio_num, uint32_t freq_hz, float duty_cycle);  

esp_err_t mcpwm_set_motor_direction(bool forward, float duty_cycle);

// esp_err_t mcpwm_set_duty_cycle(float duty_cycle);

#endif