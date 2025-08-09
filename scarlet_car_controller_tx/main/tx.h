#ifndef TX_H
#define TX_H
#pragma once

#include <stdint.h>

void espnow_tx_init(const uint8_t *peer_mac);
void espnow_tx_send(uint8_t data);

#endif