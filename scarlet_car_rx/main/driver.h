// Project Name: SW420 Vibration Sensor Module: Driver Header Code
// Author: Fayyaz Nisar Shaikh
// LinkedIn: https://www.linkedin.com/in/fayyaz-shaikh-7646312a3/
// Email: fayyaz.shaikh7862005@gmail.com
// Phone: +91 8591686239



#include <stdint.h>
#include "driver/gpio.h"

#ifndef DRIVER_H
#define DRIVER_H

int read_vibration(uint8_t gpio_number);
void sw420_init(uint8_t gpio_number);

#endif