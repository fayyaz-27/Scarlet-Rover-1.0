// Project Name: ESP-NOW Driver Code
// Author: Fayyaz Nisar Shaikh
// LinkedIn: https://www.linkedin.com/in/fayyaz-shaikh-7646312a3/
// Email: fayyaz.shaikh7862005@gmail.com
// Phone: +91 8591686239



#ifndef RX_H
#define RX_H

// rx.h
#pragma once

#include <stdint.h>

void espnow_rx_init(void);
uint8_t get_latest_rx_data(void);

#endif