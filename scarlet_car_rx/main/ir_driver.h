// Project Name: Infrared Sensor Module: Driver Header Code
// Author: Fayyaz Nisar Shaikh
// LinkedIn: https://www.linkedin.com/in/fayyaz-shaikh-7646312a3/
// Email: fayyaz.shaikh7862005@gmail.com
// Phone: +91 8591686239



#include <stdint.h>
#include "driver/gpio.h"

#ifndef IR_DRIVER_H
#define IR_DRIVER_H

int read_ir(uint8_t gpio_number);
void ir_init(uint8_t gpio_number);

#endif