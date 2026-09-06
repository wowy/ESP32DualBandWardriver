#pragma once

#include <stdint.h>

// Delay a node waits inside its rendezvous window before transmitting, so
// that nodes sharing the ESP-NOW channel spread their traffic out instead of
// all keying up at the same instant. Returns 0 when there is nothing to
// contend with or the assignment is not yet valid.
uint32_t calculateNodeStaggerOffsetMs(uint8_t node_index,
                                      uint8_t node_count,
                                      uint32_t window_ms);

// Convert a desired maximum TX power in dBm to the 0.25 dBm units taken by
// esp_wifi_set_max_tx_power, clamped to the supported [8, 84] range and
// snapped down to the nearest rung the radio actually implements.
int8_t txPowerDbmToQuarterDbm(int8_t dbm);
