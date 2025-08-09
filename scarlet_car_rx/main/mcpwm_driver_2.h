// Project Name: MCPWM Motor 2 Driver Header Code
// Author: Fayyaz Nisar Shaikh
// LinkedIn: https://www.linkedin.com/in/fayyaz-shaikh-7646312a3/
// Email: fayyaz.shaikh7862005@gmail.com
// Phone: +91 8591686239



#ifndef MCPWM_DRIVER_2_H
#define MCPWM_DRIVER_2_H

#include "esp_err.h"
#include <stdint.h>   // For uint8_t, uint32_t
#include <stdbool.h>  // For bool


esp_err_t mcpwm_motor2_set_direction(bool forward, float duty_cycle);

esp_err_t mcpwm_motor2_init(uint8_t fwd_gpio, uint8_t rev_gpio, uint32_t freq_hz, float duty_cycle);

#endif